import socket
from time import sleep

import mininet.node
import mininet.link
from mininet.log import info
from mininet.util import moveIntf
from mininet.cluster.link import RemoteLink

class WIFI (object):
    def __init__ (self, enableQos=True, rootSwitch=None, agentIP=None, agentPort=53724):
        self.rootSwitch = rootSwitch
        self.startAgent ()
        self.csock = None
        while self.csock == None:
            self.csock = self.connectAgent (agentIP, agentPort)

        self.tapBridgeIntfs = []

        self.csock.sendall ('GlobalValue.Bind ("SimulatorImplementationType", StringValue ("ns3::RealtimeSimulatorImpl"))\n')
        self.csock.sendall ('GlobalValue.Bind ("ChecksumEnabled", BooleanValue (True))\n')

        self.csock.sendall ('wifihelper = WifiHelper.Default()\n')
        self.csock.sendall ('wifihelper.SetStandard (WIFI_PHY_STANDARD_80211g)\n')
        self.csock.sendall ('phyhelper = YansWifiPhyHelper.Default()\n')
        self.csock.sendall ('channelhelper = YansWifiChannelHelper.Default()\n')
        self.csock.sendall ('phyhelper.SetChannel (channelhelper.Create())\n')
        if enableQos:
            self.csock.sendall ('machelper = QosWifiMacHelper.Default()\n')
        else:
            self.csock.sendall ('machelper = NqosWifiMacHelper.Default()\n')

        self.csock.sendall ('mobilityhelper = MobilityHelper ()\n')

        self.csock.sendall ('def run ():\n')
        self.csock.sendall ('    Simulator.Stop (Seconds (86400))\n')
        self.csock.sendall ('    Simulator.Run ()\n')

        self.csock.sendall ('nsThread = Thread (target = run)\n')
        self.csock.sendall ('tapBridges = []\n')

    def startAgent (self):
        self.rootSwitch.cmd ("/usr/bin/opennet-agent.py start")

    def stopAgent (self):
        self.rootSwitch.rcmd ("/usr/bin/opennet-agent.py stop")

    def connectAgent (self, ip, port):
        csock = socket.socket (socket.AF_INET, socket.SOCK_STREAM)
        try:
            info ('*** Connecting to opennet-agent... ')
            csock.connect ((ip, port))
        except socket.error, exc:
            info ('Failed\n')
            return None
        else:
            info ('Successed\n')
            return csock

    def start (self):
        self.csock.sendall ('if nsThread.isAlive ():\n    csock.sendall ("True")\nelse:\n    csock.sendall ("False")\n')
        while True:
            data = self.csock.recv (1024)
            if data == "True":
                info ('*** NS-3 thread is already running\n')
                return
            elif data == "False":
                info ('*** Starting NS-3 thread\n')
                break

        self.csock.sendall ('nsThread.start ()\n')

        info ('*** moveIntoNamespace\n')
        for tbIntf in self.tapBridgeIntfs:
            info ('{0} '.format (tbIntf.name))
            tbIntf.moveIntoNamespace ()
        info ('\n')

    def stop (self):
        self.csock.sendall ('Simulator.Stop (Seconds (1))\n')
        self.csock.sendall ('while nsThread.isAlive ():\n    sleep (0.1)\n')

    def clear (self):
        self.csock.sendall ('Simulator.Destroy ()\n')
        self.csock.sendall ('exit ()\n')
        self.csock.close ()
        self.stopAgent ()

    def addAdhoc (self, node, mobilityType="ns3::ConstantPositionMobilityModel", position=None, velocity=None):
        self.csock.sendall ('machelper.SetType ("ns3::AdhocWifiMac")\n')

        self.csock.sendall ('nsNode = Node ()\n')
        self.csock.sendall ('mobilityhelper.SetMobilityModel ("{0}")\n'.format (mobilityType))
        self.csock.sendall ('mobilityhelper.Install (nsNode)\n')
        if position != None:
            self.csock.sendall ('mm = nsNode.GetObject(MobilityModel.GetTypeId())\n')
            self.csock.sendall ('mm.SetPosition(Vector({0}, {1}, {2}))\n'.format (position[0], position[1], position[2]))
        if velocity != None and mobilityType == "ns3::ConstantVelocityMobilityModel":
            self.csock.sendall ('mm = nsNode.GetObject(MobilityModel.GetTypeId())\n')
            self.csock.sendall ('mm.SetVelocity(Vector({0}, {1}, {2}))\n'.format (velocity[0], velocity[1], velocity[2]))
        self.csock.sendall ('wifiDev = wifihelper.Install (phyhelper, machelper, nsNode).Get(0)\n')

        port = node.newPort ()
        intfName = "{0}-eth{1}".format (node.name, port)

        tbIntf = self.TapBridgeIntf (intfName, node, port, self.rootSwitch, self.csock)
        self.tapBridgeIntfs.append (tbIntf)

    def addAP (self, node, channelNumber=1, ssid="default-ssid", mobilityType="ns3::ConstantPositionMobilityModel", position=None, velocity=None):
        self.csock.sendall ('machelper.SetType ("ns3::ApWifiMac", "Ssid", SsidValue (Ssid("{0}")), "BeaconGeneration", BooleanValue(True), "BeaconInterval", TimeValue(Seconds(2.5)))\n'.format (ssid))
        self.csock.sendall ('phyhelper.Set ("ChannelNumber", UintegerValue ({0}))\n'.format (channelNumber))

        self.csock.sendall ('nsNode = Node ()\n')
        self.csock.sendall ('mobilityhelper.SetMobilityModel ("{0}")\n'.format (mobilityType))
        self.csock.sendall ('mobilityhelper.Install (nsNode)\n')
        if position != None:
            self.csock.sendall ('mm = nsNode.GetObject(MobilityModel.GetTypeId())\n')
            self.csock.sendall ('mm.SetPosition(Vector({0}, {1}, {2}))\n'.format (position[0], position[1], position[2]))
        if velocity != None and mobilityType == "ns3::ConstantVelocityMobilityModel":
            self.csock.sendall ('mm = nsNode.GetObject(MobilityModel.GetTypeId())\n')
            self.csock.sendall ('mm.SetVelocity(Vector({0}, {1}, {2}))\n'.format (velocity[0], velocity[1], velocity[2]))
        self.csock.sendall ('wifiDev = wifihelper.Install (phyhelper, machelper, nsNode).Get(0)\n')

        port = node.newPort ()
        intfName = "{0}-eth{1}".format (node.name, port)

        tbIntf = self.TapBridgeIntf (intfName, node, port, self.rootSwitch, self.csock)
        self.tapBridgeIntfs.append (tbIntf)

    def addSta (self, node, channelNumber=1, ssid="default-ssid", mobilityType="ns3::ConstantPositionMobilityModel", position=None, velocity=None):
        self.csock.sendall ('machelper.SetType ("ns3::StaWifiMac", "Ssid", SsidValue (Ssid("{0}")), "ScanType", EnumValue (StaWifiMac.ACTIVE))\n'.format (ssid))
        self.csock.sendall ('phyhelper.Set ("ChannelNumber", UintegerValue ({0}))\n'.format (channelNumber))

        self.csock.sendall ('nsNode = Node ()\n')
        self.csock.sendall ('mobilityhelper.SetMobilityModel ("{0}")\n'.format (mobilityType))
        self.csock.sendall ('mobilityhelper.Install (nsNode)\n')
        if position != None:
            self.csock.sendall ('mm = nsNode.GetObject(MobilityModel.GetTypeId())\n')
            self.csock.sendall ('mm.SetPosition(Vector({0}, {1}, {2}))\n'.format (position[0], position[1], position[2]))
        if velocity != None and mobilityType == "ns3::ConstantVelocityMobilityModel":
            self.csock.sendall ('mm = nsNode.GetObject(MobilityModel.GetTypeId())\n')
            self.csock.sendall ('mm.SetVelocity(Vector({0}, {1}, {2}))\n'.format (velocity[0], velocity[1], velocity[2]))
        self.csock.sendall ('wifiDev = wifihelper.Install (phyhelper, machelper, nsNode).Get(0)\n')

        port = node.newPort ()
        intfName = "{0}-eth{1}".format (node.name, port)

        tbIntf = self.TapBridgeIntf (intfName, node, port, self.rootSwitch, self.csock)
        self.tapBridgeIntfs.append (tbIntf)

    class TapBridgeIntf (mininet.link.Intf):
        """
        TapBridgeIntf is a Linux TAP interface, which is bridged with an NS-3 NetDevice.
        """
        def __init__ (self, name=None, node=None, port=None, localNode=None, csock=None, **params):
            self.name = name
            self.node = node
            self.localNode = localNode
            self.csock = csock
            self.createTap (self.name)
            self.delayedMove = True
            if node.inNamespace == True:
                self.inRightNamespace = False
            else:
                self.inRightNamespace = True
            mininet.link.Intf.__init__ (self, name, node, port, **params)

            self.csock.sendall ('nsDevice = wifiDev\n')

            self.csock.sendall ('tapBridgeHelper = TapBridgeHelper ()\n')
            self.csock.sendall ('tapBridgeHelper.SetAttribute ("Mode", StringValue ("UseLocal"))\n')
            self.csock.sendall ('tapBridgeHelper.SetAttribute ("DeviceName", StringValue ("{0}"))\n'.format (self.name))
            self.csock.sendall ('macAddress = Mac48Address.Allocate ()\n')
            self.csock.sendall ('tapBridgeHelper.SetAttribute ("MacAddress", Mac48AddressValue (macAddress))\n')
            self.csock.sendall ('tb = tapBridgeHelper.Install (nsNode, nsDevice)\n')
            self.csock.sendall ('tapBridges.append (tb)\n')

        def moveIntoNamespace (self):
            while True:
                self.csock.sendall ('if tapBridges[-1].IsLinkUp():\n    csock.sendall ("True")\nelse:\n    csock.sendall ("False")\n')
                data = self.csock.recv (1024)
                if data == "True":
                    break
                else:
                    sleep (0.1)

            RemoteLink.moveIntf (self.name, self.node)

            self.node.cmd ('ip link set dev {0} up'.format (self.name))
            self.node.cmd ('ip addr add dev {0} {1}/{2}'.format (self.name, self.ip, self.prefixLen))

        def cmd (self, *args, **kwargs):
            if self.inRightNamespace == True:
                return self.node.cmd (*args, **kwargs)
            else:
                return self.localNode.cmd (*args, **kwargs)

        def createTap (self, name):
            self.node.cmd ('ip tuntap add {0} mode tap'.format (name))
            self.node.cmd ('ip link set dev {0} netns 1'.format (name))
