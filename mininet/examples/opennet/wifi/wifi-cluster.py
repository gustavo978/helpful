# wifi-cluster.py

import sys
from os import popen

from subprocess import call

from mininet.cluster.net import MininetCluster
from mininet.cluster.cli import ClusterCLI
from mininet.log import setLogLevel, info
from mininet.wifi import *
from mininet.opennet import *

AP = 1
STA = 2
ADHOC = 2
IP = ""

def emulation ():

    # Use MininetCluster for cluster emulation of hosts and switches
    servers = ['master']
    # For multiple hosts: servers = ['master', 'slave']

    # Do not use 127.0.0.1
    serverIP = {'master': IP}
    # For multiple hosts: serverIP = {'master': "192.168.0.1", 'slave': "192.168.0.2"}
    net = MininetCluster (controller=None, user='root', servers=servers, serverIP=serverIP)

    # Add OpenFlow Switches (OVS)
    info ("*** Add OFS\n")
    rootSwitch = net.addSwitch ("sw0", failMode='standalone', user='root', server='master', serverIP=IP)
    # For multiple hosts:
    # rootSwitch1 = net.addSwitch ("sw1", failMode='standalone', user='root', server='master', serverIP="192.168.0.1")
    # rootSwitch2 = net.addSwitch ("sw2", failMode='standalone', user='root', server='slave', serverIP="192.168.0.2")

    wifiList = []

    stationIP = {"h1": "10.0.0.1", "h2": "10.0.0.2"}
    adhocIP = {"adhoc1": "10.0.1.1", "adhoc2": "10.0.1.2", "adhoc3": "10.0.1.3"}

    # Reference mininet/wifi.py for more detail of WIFI class
    wifi = WIFI (enableQos=True, rootSwitch=rootSwitch, agentIP=IP)
    # For multiple hosts, multiple WIFI object is required:
    # wifi1 = WIFI (enableQos=True, rootSwitch=rootSwitch1, agentIP="192.168.0.1")
    # wifi2 = WIFI (enableQos=True, rootSwitch=rootSwitch2, agentIP="192.168.0.2")

    info ('*** Add Access Point on WIFI\n')
    for i in range (1, AP + 1):
        name = "ap{0}".format (i)
        info (name + " ")
        sw = net.addSwitch (name, failMode='standalone', user='root', server='master', serverIP=IP)
        wifi.addAP (sw, channelNumber=1, ssid="opennet")

        # For multiple hosts:
        # sw2 = net.addSwitch ("ap2", failMode='standalone', user='root', server='slave', serverIP="192.168.0.2")
        # wifi2.addAP (sw2, channelNumber=1, ssid="opennet")
    info ('\n')

    info ('*** Add Station on WIFI\n')
    for i in range (1, STA + 1):
        name = "h{0}".format (i)
        info (name + " ")
        # Station is combination of Mininet host and NS-3 node
        host = net.addHost (name, ip=stationIP[name], user='root', server='master', serverIP=IP)
        wifi.addSta (host, channelNumber=1, ssid="opennet")

        # The following line is an example about using mobility model
        # wifi.addSta (host, channelNumber=1, ssid="opennet", mobilityType="ns3::ConstantVelocityMobilityModel", velocity=[0,0,0], position=[0,0,0])

        # For multiple hosts:
        # host = net.addHost ("sta2", ip=stationIP["sta2"], user='root', server='slave', serverIP="192.168.0.2")
        # wifi2.addSta (host, channelNumber=1, ssid="opennet")
    info ('\n')

    info ('*** Add Adhoc on WIFI {0}\n')
    for i in range (1, ADHOC + 1):
        name = "adhoc{0}".format (i)
        info (name + " ")
        host = net.addHost (name, ip=adhocIP[name], user='root', server='master', serverIP=IP)
        wifi.addAdhoc (host)

        # The following line is an example about using mobility model
        # wifi.addAdhoc (host, mobilityType="ns3::ConstantVelocityMobilityModel", velocity=[0,0,0], position=[0,0,0])

        # For multiple hosts:
        # host = net.addHost ("adhoc2", ip=adhocIP[name], user='root', server='slave', serverIP="192.168.0.2")
        # wifi2.addAdhoc (host)
    info ('\n')

    wifiList.append (wifi)

    info ('*** net.start ()\n')
    net.start ()

    info ('*** wifi.start ()\n')
    for wifi in wifiList:
        wifi.start ()

    ClusterCLI (net)

    for wifi in wifiList:
        wifi.stop ()
        wifi.clear ()
    net.stop ()

if __name__ == '__main__':

    IP = getIntfAddr ('eth0')

    setLogLevel ('info')
    emulation ()
