# lte-example.py

import sys
from os import popen

from subprocess import call

from mininet.cluster.net import MininetCluster
from mininet.cluster.cli import ClusterCLI
from mininet.log import setLogLevel, info
from mininet.lte import *
from mininet.opennet import getIntfAddr

ENB = 2
UE = 2
TDF = 100
CPU = 1
IP = ''
distance=1000.0

def emulation ():

    # Use MininetCluster for cluster emulation of hosts and switches
    servers = ['master']
    # For multiple hosts: servers = ['master', 'slave']

    # Do not use 127.0.0.1
    serverIP = {'master': IP}
    # For multiple hosts: serverIP = {'master': "192.168.0.1", 'slave': "192.168.0.2"}
    net = MininetCluster (controller=None, user='root', servers=servers, serverIP=serverIP)
    switches = [None]

    # Add OpenFlow Switches (OVS)
    info ("*** Add OFS\n")
    switch = net.addSwitch ("ofs1", failMode='standalone', user='root', server='master', serverIP=IP)
    switches.append (switch)

    switch = net.addSwitch ("ofs2", failMode='standalone', user='root', server='master', serverIP=IP)
    switches.append (switch)

    # For multiple hosts:
    # switch = net.addSwitch ("ofs3", failMode='standalone', user='root', server='slave', serverIP="192.168.0.2")
    # switches.append (switch)

    info ("*** Add link between OFS\n")
    net.addLink (switches[1], switches[2])

    # For multiple hosts:
    # net.addLink (switches[1], switches[3])

    hosts = []
    ueIpList = []
    lteList = []

    # Create one LTE object for each available CPU
    for c in range (0, CPU):
        # Master will responsible for emulation of EPC (MME/SGW/PGW)
        if c == 0:
            MODE = 'Master'
        else:
            MODE = 'Slave'

        LOG = "/tmp/ns-3-log-{0}.txt".format (c)

        # IP address format of UE
        UE_IP_BASE = '7.0.{0}.1'.format (c)

        # The TAP interface which is used as junction point of Mininet and NS-3
        SLAVE_DEVICE = 'slaveTap{0}'.format (c)

        # Reference mininet/lte.py for more detail of Lte class
        lte = Lte (tdf=TDF, mode=MODE, epcSwitch=switches[1], agentIp=IP, logFile=LOG,
                   imsiBase=c * (UE / CPU), cellIdBase=c * (ENB / CPU), ueIpBase=UE_IP_BASE, slaveName=SLAVE_DEVICE)
        # For multiple hosts: agentIp="192.168.0.2"

        info ('*** Add eNodeB on LTE {0}\n'.format (c))
        for i in range (c * (ENB / CPU) + 1, (c + 1) * (ENB / CPU) + 1):
            info ('enbTap{0} '.format (i))

            lte.addEnb (switches[2], "enbTap{0}".format (i), position=[distance * i, distance * i * c, 0])

            # For multiple hosts: agentIp="192.168.0.2"
            # lte.addEnb (switches[3], "enbTap{0}".format (i), position=[distance * i, distance * i * c, 0])
        info ('\n')

        info ('*** Add UE on LTE {0}\n'.format (c))
        for i in range (c * (UE / CPU) + 1, (c + 1) * (UE / CPU) + 1):
            info ('h{0} '.format (i))

            # UE is combination of Mininet host and NS-3 node
            host = net.addHost ("h{0}".format (i), user='root', server='master', serverIP=IP, tdf=TDF)
            # For multiple hosts:
            # host = net.addHost ("h{0}".format (i), user='root', server='slave', serverIP="192.168.0.2", tdf=TDF)

            ueIP, ueIndex = lte.addUe (host, position=[distance * i, distance * i * c, 0])

            # Add EpsBearer to UE
            lte.addEpsBearer (ueIndex=ueIndex, localPortStart=3000, localPortEnd=3999, qci='EpsBearer.NGBR_IMS')
            lte.addEpsBearer (ueIndex=ueIndex, remotePortStart=3000, remotePortEnd=3999, qci='EpsBearer.NGBR_IMS')
            lte.addEpsBearer (ueIndex=ueIndex, localPortStart=4000, localPortEnd=4999, qci='EpsBearer.GBR_CONV_VOICE')
            lte.addEpsBearer (ueIndex=ueIndex, remotePortStart=4000, remotePortEnd=4999, qci='EpsBearer.GBR_CONV_VOICE')

            # Record IP address for further usage (depends on scenario)
            ueIpList.append (ueIP)
            hosts.append (host)
        info ('\n')

        lteList.append (lte)

    info ('*** net.start ()\n')
    net.start ()

    info ('*** lte.start ()\n')
    for lte in lteList:
        lte.start ()

    info ('*** Please wait for activation of EPS bearer...\n')
    call ( 'sleep 15', shell=True )

    info ('*** Notification: GTP-U tunnel does not support ping\n')
    # Emulation example:
    #  h1 iperf3 -s
    #  h2 iperf3 -c h1 -M 1024 -R -t 3
    #  or h2 iperf3 -c h1 -u -l 1024 -R -t 3 -b 1M
    ClusterCLI (net)

    for lte in lteList:
        lte.stop ()
        lte.clear ()
    net.stop ()

if __name__ == '__main__':
    # example: python lte-example.py 2 2 100 1

    if len (sys.argv) == 5:
        ENB = int (sys.argv[1])
        UE = int (sys.argv[2])
        TDF = int (sys.argv[3])
        CPU = int (sys.argv[4])

    IP = getIntfAddr ('eth0')

    setLogLevel ('info')
    emulation ()
