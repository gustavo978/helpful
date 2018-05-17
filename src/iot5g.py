from grid import *
from antenna import Antenna
from controller import *
from bbu import *
from cluster import *
from sensor import *
from application import *
from lora_sensor import *
import util

import simpy
import random
import numpy as np
import math
import sys
import matplotlib.pyplot as plt
import os

DMACROMACRO             = 500
DMACROUE                = 30
DMACROCLUSTER           = 90
DSMALLUE                = 5
DSMALLSMALL             = 10
DROPRADIUS_MC           = 250
DROPRADIUS_SC           = 500
DROPRADIUS_SC_CLUSTER   = 70
DROPRADIUS_UE_CLUSTER   = 70
DSMALLUE                = 5
MAX_DELTA               = 1
MAX_REP                 = 1
MAX_I                   = 50


packetsAtBS = []
bsId = 1
nrCollisions = 0
nrReceived = 0
nrProcessed = 0
nrLost = 0
full_collision = True
grid = None

sf7 = np.array([7,-126.5,-124.25,-120.75])
sf8 = np.array([8,-127.25,-126.75,-124.0])
sf9 = np.array([9,-131.25,-128.25,-127.5])
sf10 = np.array([10,-132.75,-130.25,-128.75])
sf11 = np.array([11,-134.5,-132.75,-128.75])
sf12 = np.array([12,-133.25,-132.25,-132.25])
sensi = np.array([sf7,sf8,sf9,sf10,sf11,sf12])

maxBSReceives = 8

"""
Functions to build de scneario
"""
def build_scenario(n_bbu, n_bs, n_clusters, n_ue):
    grid = Grid(size=(2000,2000))
    macrocells_center = list()

    cntrl = Controller(grid, control_network=False)
    grid.add_controller(cntrl)

    for i in range(n_bbu):
        rpos = grid.random_pos()
        bbu = BBU(pos=rpos, controller=cntrl, grid=grid)
        grid.add_bbu(bbu)

    macrocells(grid, DMACROMACRO, n_bs,  macrocells_center)
    clusters(grid, macrocells_center, n_clusters)
    users(grid, macrocells_center, n_bs, n_clusters, n_ue)

    return grid

def macrocells(grid, radius, n_bs, macrocells_center):
    center = numpy.array([grid.size[0]/2, grid.size[1]/2])

    #Center Antenna
    macrocells_center.append((grid.size[0]/2, grid.size[1]/2))
    bs = Antenna(0, Antenna.BS_ID, center, None, grid)
    grid.add_antenna(bs)

def clusters(grid, macrocells_center, n_clusters):
    count_clusters = 0
    p_clusters = list()
    reset = 0;

    for i in range(0, len(macrocells_center)):
        count_clusters = 0
        #print("Create macrocells cluster and rhh: " + str(i))

        for l in range (0, n_clusters):
            pos = generate_xy(macrocells_center[i],
                    DMACROMACRO*0.425, DMACROCLUSTER)
            p_clusters.append(pos)

            cluster = Cluster(l+1, p_clusters[l], grid)
            grid.add_cluster(cluster)

def distance(p1, p2):
    return math.sqrt((p1[0] - p2[0]) ** 2 + (p1[1] - p2[1]) ** 2)

def users(grid, macrocells_center, n_bs, n_clusters, n_ue):
    count_ue = 0
    p_users = list()

    for i in range(0, n_bs):
        reset = 1001
        count_ue = 0
        while (count_ue < n_ue):
            
            p_is_ok = True
            if reset > 1000:
                count_ue = 0
                reset = 0
                p_users = list()

            if n_clusters > 0:
                x = (i*n_clusters)
                y = ((i*n_clusters) + n_clusters)-1
                r = random.randint(x,y)
                #print("x: " + str(x) + " y: " + str(y) + " r: " + str(r))
                cluster = grid._clusters[r]

            #Define type of user
            #if random.random() < 0.666 and n_clusters > 0:
            #    p = generate_xy(cluster._pos, DROPRADIUS_UE_CLUSTER, 0)
            #    p_is_ok = is_possition_ok(p, cluster._pos, DSMALLSMALL)
            #else:
            p = generate_xy(macrocells_center[i], DMACROMACRO*0.425, DMACROUE)
            
            #Distribution
            if not(p_is_ok):
                    reset = reset + 1
            else:
                count_ue = count_ue + 1
                #print p
                p_users.append(p)
            
        for j in range(0,len(p_users)):
            app = Application(1,30000)
            dist = distance(grid._antennas[0]._pos, p_users[j]) 
            u = Sensor(j+1, p_users[j], None, 1,app, dist)
            grid.add_user(u)

def generate_xy(center, radius, min_distance):
    pos = [None] * 2 
    not_done = True
    while not_done:
        pos[0] = radius * (1 - 2 * random.random()) + center[0]
        pos[1] = radius * (1 - 2 * random.random()) + center[1]
        not_done = euclidian(pos, center) < min_distance

    return pos

def euclidian(a,b):
   return scipy.spatial.distance.euclidean(a,b)

def is_possition_ok(p, vector, min_distance):
    result = True
    if len(vector) != 0:
        for i in range(0, len(vector)):
            d = euclidian(p,vector[i])
            if  (d < min_distance) or (d == 0):
                result = False
    return result

"""
Simulation functions
"""

def frequencyCollision(p1,p2):
    if (abs(p1.freq-p2.freq)<=120 and (p1.bw==500 or p2.freq==500)):
        return True
    elif (abs(p1.freq-p2.freq)<=60 and (p1.bw==250 or p2.freq==250)):
        return True
    else:
        if (abs(p1.freq-p2.freq)<=30):
            return True
    return False

def checkcollision(packet):
    col = 0 # flag needed since there might be several collisions for packet
    processing = 0
    for i in range(0,len(packetsAtBS)):
        if packetsAtBS[i].packet.processed == 1:
            processing = processing + 1
    if (processing > maxBSReceives):
        #print "too long:", len(packetsAtBS)
        packet.processed = 0
    else:
        packet.processed = 1

    if packetsAtBS:
        #print "CHECK node {} (sf:{} bw:{} freq:{:.6e}) others: {}".format(
             #packet.nodeid, packet.sf, packet.bw, packet.freq,
             #len(packetsAtBS))
        for other in packetsAtBS:
            if other._id != packet.nodeid:
               #print ">> node {} (sf:{} bw:{} freq:{:.6e})".format(
                   #other._id, other.packet.sf, other.packet.bw, other.packet.freq)
               # simple collision
               if frequencyCollision(packet, other.packet) \
                   and sfCollision(packet, other.packet):
                   if full_collision:
                       if timingCollision(packet, other.packet):
                           # check who collides in the power domain
                           c = powerCollision(packet, other.packet)
                           # mark all the collided packets
                           # either this one, the other one, or both
                           for p in c:
                               p.collided = 1
                               if p == packet:
                                   col = 1
                       else:
                           # no timing collision, all fine
                           pass
                   else:
                       packet.collided = 1
                       other.packet.collided = 1  # other also got lost, if it wasn't lost already
                       col = 1
        return col
    return 0

def frequencyCollision(p1,p2):
    if (abs(p1.freq-p2.freq)<=120 and (p1.bw==500 or p2.freq==500)):
        #print "frequency coll 500"
        return True
    elif (abs(p1.freq-p2.freq)<=60 and (p1.bw==250 or p2.freq==250)):
        #print "frequency coll 250"
        return True
    else:
        if (abs(p1.freq-p2.freq)<=30):
            #print "frequency coll 125"
            return True
        #else:
    #print "no frequency coll"
    return False

def sfCollision(p1, p2):
    if p1.sf == p2.sf:
        #print "collision sf node {} and node {}".format(p1.nodeid, p2.nodeid)
        # p2 may have been lost too, will be marked by other checks
        return True
    #print "no sf collision"
    return False

def powerCollision(p1, p2):
    powerThreshold = 6 # dB
    #print "pwr: node {0.nodeid} {0.rssi:3.2f} dBm node {1.nodeid} {1.rssi:3.2f} dBm; diff {2:3.2f} dBm".format(p1, p2, round(p1.rssi - p2.rssi,2))
    if abs(p1.rssi - p2.rssi) < powerThreshold:
        #print "collision pwr both node {} and node {}".format(p1.nodeid, p2.nodeid)
        # packets are too close to each other, both collide
        # return both packets as casualties
        return (p1, p2)
    elif p1.rssi - p2.rssi < powerThreshold:
        # p2 overpowered p1, return p1 as casualty
        #print "collision pwr node {} overpowered node {}".format(p2.nodeid, p1.nodeid)
        return (p1,)
    #print "p1 wins, p2 lost"
    # p2 was the weaker packet, return it as a casualty
    return (p2,)

def timingCollision(p1, p2):
    # assuming p1 is the freshly arrived packet and this is the last check
    # we've already determined that p1 is a weak packet, so the only
    # way we can win is by being late enough (only the first n - 5 preamble symbols overlap)

    # assuming 8 preamble symbols
    Npream = 8

    # we can lose at most (Npream - 5) * Tsym of our preamble
    Tpreamb = 2**p1.sf/(1.0*p1.bw) * (Npream - 5)

    # check whether p2 ends in p1's critical section
    p2_end = p2.addTime + p2.rectime
    p1_cs = env.now + Tpreamb
    #print "collision timing node {} ({},{},{}) node {} ({},{})".format(
        #p1.nodeid, env.now - env.now, p1_cs - env.now, p1.rectime,
        #p2.nodeid, p2.addTime - env.now, p2_end - env.now
    #)
    if p1_cs < p2_end:
        # p1 collided with p2 and lost
        #print "not late enough"
        return True
    #print "saved by the preamble"
    return False


def transmit(env,node):
    while True:
        yield env.timeout(random.expovariate(1.0/float(node.application.period)))

        # time sending and receiving
        # packet arrives -> add to base station

        # LoRa Connect
        #print "ADD"
        node.stablish_connection(grid.bs_list[0])
        grid.bs_list[0].connect(node)
        #plot_grid(grid)

        node.sent = node.sent + 1
        if (node in packetsAtBS):
            print ("ERROR: packet already in")
        else:
            sensitivity = sensi[node.packet.sf - 7,[125,250,500].index(node.packet.bw) + 1]

            #print node.packet.rectime
            if node.packet.rssi < sensitivity:
                #print "node {}: packet will be lost".format(node._id)
                node.packet.lost = True
            else:
                node.packet.lost = False
                # adding packet if no collision
                if (checkcollision(node.packet)==1):
                    node.packet.collided = 1
                else:
                    node.packet.collided = 0
                packetsAtBS.append(node)
                node.packet.addTime = env.now

        yield env.timeout(node.packet.rectime)

        if node.packet.lost:
            global nrLost
            nrLost += 1
        if node.packet.collided == 1:
            global nrCollisions
            nrCollisions = nrCollisions +1
        if node.packet.collided == 0 and not node.packet.lost:
            global nrReceived
            nrReceived = nrReceived + 1
        if node.packet.processed == 1:
            global nrProcessed
            nrProcessed = nrProcessed + 1

        # complete packet has been received by base station
        # can remove it
        if (node in packetsAtBS):
            packetsAtBS.remove(node)
            node.disconect()
            grid.bs_list[0].disconnect(node)
            # reset the packet

        node.packet.collided = 0
        node.packet.processed = 0
        node.packet.lost = False

"""
Main
"""
if __name__ == "__main__":

    bbu = 1 
    bs = 1
    #ues = [250,500, 1000, 5000]
    ues = [50]
    cluster = 1
    index = 1

    for ue in ues:
        for i in range (0, index):
            grid = build_scenario(bbu, bs, cluster, ue)
            plot_grid(grid)

            env = simpy.Environment()

            # Beguin simulation
            for sensor in grid._user:
                env.process(transmit(env, sensor))
            env.run(until=3600000)

            sent = sum(n.sent for n in grid._user)

            fname = "exp.dat"
            print (fname)
            if os.path.isfile(fname):
                res = "\n" + str(i) + "," + str(ue) + "," + str(sent) + "," + str(nrReceived) + "," + str(nrProcessed) + "," + str(nrCollisions) + "," + str(nrLost)
            else:
                res  = "iteracao,nrNodes,nrTransmissions,nrReceived,nrProcessed,nrCollisions,nrLost\n" + str(i) + "," + str(ue) + "," + str(sent) + "," + str(nrReceived) + "," + str(nrProcessed) + "," + str(nrCollisions) + "," + str(nrLost)
            with open(fname, "a") as myfile:
                myfile.write(res)
            myfile.close()

