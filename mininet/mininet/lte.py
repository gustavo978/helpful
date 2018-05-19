import re
import socket
from time import sleep

import mininet.node
import mininet.link
from mininet.log import info
from mininet.util import moveIntf
from mininet.cluster.link import RemoteLink

class Lte (object):
    def __init__ (self, tdf=1, mode='Master', imsiBase=0, cellIdBase=0,
                  ueIpBase='7.0.0.1', ueGwIpAddr='7.0.0.1',
                  pgwIpBase='1.0.0.0', pgwMask='255.0.0.0',
                  epcSwitch=None, agentIp=None, agentPort=53724, logFile=None,
                  homeEnbTxPower=30.0, slaveName='slaveTap'):

        if epcSwitch == None:
            info ('*** error: epcSwitch is a required argument.\n')
            return
        elif agentIp == None:
            info ('*** error: agentIp is a required argument.\n')
            return

        self.epcSwitch = epcSwitch
        self.ueIpBase = ueIpBase
        self.ueGwIpAddr = ueGwIpAddr
        self.tapBridgeIntfs = []
        self.ueIndex = -1

        self.startAgent ()
        self.csock = None
        while self.csock == None:
            self.csock = self.connectAgent (agentIp, agentPort)

        if mode == 'Master':
            self.addEpcEntity (self.epcSwitch, 'pgwTap')
            self.addEpcEntity (self.epcSwitch, 'sgwTap')
            self.addEpcEntity (self.epcSwitch, 'mmeTap')
            self.addEpcEntity (self.epcSwitch, 'masterTap')
            self.nextAddr = 2
        elif mode == 'Slave':
            IpBase = re.sub (r'[0-9]*\.([0-9]*\.[0-9]*\.[0-9])', r'0.\1', ueIpBase)
            self.csock.sendall ('Config.SetDefault ("ns3::TapEpcHelper::EpcSlaveDeviceName", StringValue ("{0}"))\n'.format (slaveName))
            self.csock.sendall ('Config.SetDefault ("ns3::TapEpcHelper::SlaveUeIpAddressBase", StringValue ("{0}"))\n'.format (IpBase))
            self.csock.sendall ('Config.SetDefault ("ns3::TapEpcHelper::SlaveIpAddressBase", StringValue ("{0}"))\n'.format (IpBase))
            self.addEpcEntity (self.epcSwitch, slaveName)
            self.nextAddr = 1
        else:
            info ('*** error: mode should be Master or Slave.\n')
            self.csock.sendall ("exit")
            return

        self.csock.sendall ('LogComponentEnable ("TapEpcHelper", LOG_LEVEL_ALL)\n')
        self.csock.sendall ('LogComponentEnable ("TapEpcMme", LOG_LEVEL_ALL)\n')
        self.csock.sendall ('LogComponentEnable ("EpcSgwPgwApplication", LOG_LEVEL_ALL)\n')
        self.csock.sendall ('LogComponentEnable ("FdNetDevice", LOG_LEVEL_DEBUG)\n')
        self.csock.sendall ('LogComponentEnable ("TeidDscpMapping", LOG_LEVEL_LOGIC)\n')
        self.csock.sendall ('LogComponentEnable ("TapEpcEnbApplication", LOG_LEVEL_ALL)\n')

        self.csock.sendall ('GlobalValue.Bind ("SimulatorImplementationType", StringValue ("ns3::RealtimeSimulatorImpl"))\n')
        self.csock.sendall ('GlobalValue.Bind ("ChecksumEnabled", BooleanValue (True))\n')

        self.csock.sendall ('Config.SetDefault ("ns3::LteSpectrumPhy::CtrlErrorModelEnabled", BooleanValue (False))\n')
        self.csock.sendall ('Config.SetDefault ("ns3::LteSpectrumPhy::DataErrorModelEnabled", BooleanValue (False))\n')
        self.csock.sendall ('Config.SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (2440))\n')
        self.csock.sendall ('Config.SetDefault ("ns3::LteHelper::Scheduler", StringValue ("ns3::FdMtFfMacScheduler"))\n')
        self.csock.sendall ('Config.SetDefault ("ns3::TapEpcHelper::Mode", StringValue ("{0}"))\n'.format (mode))
        self.csock.sendall ('Config.SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue ({0}))\n'.format (homeEnbTxPower))

        self.csock.sendall ('LteTimeDilationFactor.SetTimeDilationFactor ({0})\n'.format (tdf))
        
        if logFile != None:
            self.csock.sendall ('Config.SetDefault ("ns3::TapEpcHelper::LogFile", StringValue ("{0}"))\n'.format (logFile))

        self.csock.sendall ('attachDelay = 10.0\n')

        self.csock.sendall ('lteHelper = LteHelper ()\n')
        self.csock.sendall ('lteHelper.SetImsiCounter ({0})\n'.format (imsiBase))
        self.csock.sendall ('lteHelper.SetCellIdCounter ({0})\n'.format (cellIdBase))

        self.csock.sendall ('tapEpcHelper = TapEpcHelper ()\n')
        self.csock.sendall ('lteHelper.SetEpcHelper (tapEpcHelper)\n')
        self.csock.sendall ('tapEpcHelper.Initialize ()\n')

        if mode == 'Master':
            self.csock.sendall ('pgw = tapEpcHelper.GetPgwNode ()\n')

            self.csock.sendall ('tap = TapFdNetDeviceHelper ()\n')
            self.csock.sendall ('tap.SetDeviceName ("pgwTap")\n')
            self.csock.sendall ('tap.SetTapMacAddress (Mac48Address.Allocate ())\n')
            self.csock.sendall ('pgwDevice = tap.Install (pgw)\n')

            self.csock.sendall ('ipv4Helper = Ipv4AddressHelper ()\n')
            self.csock.sendall ('ipv4Helper.SetBase (Ipv4Address ("{0}"), Ipv4Mask ("{1}"))\n'.format (pgwIpBase, pgwMask))
            self.csock.sendall ('pgwIpIfaces = ipv4Helper.Assign (pgwDevice)\n')

        self.csock.sendall ('mobility = MobilityHelper ()\n')

        self.csock.sendall ('enbLteDevs = NetDeviceContainer ()\n')
        self.csock.sendall ('ueLteDevs = NetDeviceContainer ()\n')

        self.csock.sendall ('internetStack = InternetStackHelper ()\n')
        self.csock.sendall ('internetStack.SetIpv6StackInstall (False)\n')

        self.csock.sendall ('Simulator.Schedule (Seconds (attachDelay), LteHelper.Attach, lteHelper, ueLteDevs)\n')

        self.csock.sendall ('def run ():\n')
        self.csock.sendall ('    Simulator.Stop (Seconds (86400))\n')
        self.csock.sendall ('    Simulator.Run ()\n')

        self.csock.sendall ('nsThread = Thread (target = run)\n')
        self.csock.sendall ('tapBridges = []\n')

    def startAgent (self):
        self.epcSwitch.rcmd ("/usr/bin/opennet-agent.py start")

    def stopAgent (self):
        self.epcSwitch.rcmd ("/usr/bin/opennet-agent.py stop")

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

    def addEpcEntity (self, node, intfName):
        port = node.newPort ()
        self.TapIntf (intfName, node, port)

    def addEnb (self, node, intfName, mobilityType="ns3::ConstantPositionMobilityModel", position=None, velocity=None):
        port = node.newPort ()
        self.TapIntf (intfName, node, port)

        self.csock.sendall ('nsNode = Node ()\n')

        self.csock.sendall ('mobility.SetMobilityModel ("{0}")\n'.format (mobilityType))
        self.csock.sendall ('mobility.Install (nsNode)\n')
        if position != None:
            self.csock.sendall ('mm = nsNode.GetObject(MobilityModel.GetTypeId())\n')
            self.csock.sendall ('mm.SetPosition(Vector({0}, {1}, {2}))\n'.format (position[0], position[1], position[2]))
        if velocity != None and mobilityType == "ns3::ConstantVelocityMobilityModel":
            self.csock.sendall ('mm = nsNode.GetObject(MobilityModel.GetTypeId())\n')
            self.csock.sendall ('mm.SetVelocity(Vector({0}, {1}, {2}))\n'.format (velocity[0], velocity[1], velocity[2]))

        self.csock.sendall ('enbLteDev = lteHelper.InstallEnbDevice (NodeContainer (nsNode))\n')
        self.csock.sendall ('enbLteDevs.Add (enbLteDev)\n')

    def addUe (self, node, mobilityType="ns3::ConstantPositionMobilityModel", position=None, velocity=None):
        self.ueIndex += 1
        node.cmd ('sysctl -w net.ipv6.conf.all.disable_ipv6=1')
        port = node.newPort ()
        intfName = "{0}-eth{1}".format (node.name, port)

        self.csock.sendall ('nsNode = Node ()\n')

        self.csock.sendall ('mobility.SetMobilityModel ("{0}")\n'.format (mobilityType))
        self.csock.sendall ('mobility.Install (nsNode)\n')
        if position != None:
            self.csock.sendall ('mm = nsNode.GetObject(MobilityModel.GetTypeId())\n')
            self.csock.sendall ('mm.SetPosition(Vector({0}, {1}, {2}))\n'.format (position[0], position[1], position[2]))
        if velocity != None and mobilityType == "ns3::ConstantVelocityMobilityModel":
            self.csock.sendall ('mm = nsNode.GetObject(MobilityModel.GetTypeId())\n')
            self.csock.sendall ('mm.SetVelocity(Vector({0}, {1}, {2}))\n'.format (velocity[0], velocity[1], velocity[2]))

        self.csock.sendall ('ueLteDev = lteHelper.InstallUeDevice (NodeContainer (nsNode))\n')
        self.csock.sendall ('ueLteDevs.Add (ueLteDev)\n')

        self.csock.sendall ('internetStack.Install (nsNode)\n')
        self.csock.sendall ('tapEpcHelper.AssignUeIpv4Address (ueLteDev)\n')

        self.csock.sendall ('gatewayMacAddr = tapEpcHelper.GetUeDefaultGatewayMacAddress ()\n')

        ueIp = self.allocateIp ()
        tbIntf = self.TapBridgeIntf (intfName, node, port, self.ueGwIpAddr, ueIp, self.epcSwitch, self.csock)
        self.tapBridgeIntfs.append (tbIntf)
        return ueIp, self.ueIndex

    def addEpsBearer (self, ueIndex=0, localPortStart=0, localPortEnd=65535, remotePortStart=0, remotePortEnd=65535, qci='EpsBearer.NGBR_VIDEO_TCP_DEFAULT'):
        self.csock.sendall ('tft = EpcTft ()\n')
        self.csock.sendall ('pf = EpcTft.PacketFilter ()\n')
        self.csock.sendall ('pf.localPortStart = {0}\n'.format (localPortStart))
        self.csock.sendall ('pf.localPortEnd = {0}\n'.format (localPortEnd))
        self.csock.sendall ('pf.remotePortStart = {0}\n'.format (remotePortStart))
        self.csock.sendall ('pf.remotePortEnd = {0}\n'.format (remotePortEnd))
        self.csock.sendall ('tft.Add (pf)\n')
        self.csock.sendall ('bearer = EpsBearer ({0})\n'.format (qci))
        self.csock.sendall ('Simulator.Schedule (Seconds (attachDelay), LteHelper.ActivateDedicatedEpsBearer, lteHelper, ueLteDevs.Get ({0}), bearer, tft)\n'.format (ueIndex))

    def allocateIp (self):
        pat = '[0-9]*\.[0-9]*\.[0-9]*\.'
        base = (re.findall (pat, self.ueIpBase))[0]
        ip = "{0}{1}".format (base, self.nextAddr)
        self.nextAddr += 1
        return ip

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

        self.disableIpv6 (self.epcSwitch)

        self.csock.sendall ('nsThread.start ()\n')

        info ('*** moveIntoNamespace\n')
        for tbIntf in self.tapBridgeIntfs:
            info ('{0} '.format (tbIntf.name))
            tbIntf.moveIntoNamespace ()
        info ('\n')
        
        self.enableIpv6 (self.epcSwitch)

    def stop (self):
        self.csock.sendall ('Simulator.Stop (Seconds (1))\n')
        self.csock.sendall ('while nsThread.isAlive ():\n    sleep (0.1)\n')

    def clear (self):
        self.csock.sendall ('Simulator.Destroy ()\n')
        self.csock.sendall ('exit ()\n')
        self.csock.close ()
        self.stopAgent ()

    def disableIpv6 (self, node):
        node.rcmd ('sysctl -w net.ipv6.conf.all.disable_ipv6=1')

    def enableIpv6 (self, node):
        node.rcmd ('sysctl -w net.ipv6.conf.all.disable_ipv6=0')

    class TapIntf (mininet.link.Intf):
        """
        TapIntf is a Linux TAP interface.
        """
        def __init__ (self, name=None, node=None, port=None, **params):
            self.name = name
            self.node = node
            self.createTap (self.name)
            mininet.link.Intf.__init__ (self, self.name, node, port, **params)

        def createTap (self, name):
            self.node.cmd ('ip tuntap add {0} mode tap'.format (name))

    class TapBridgeIntf (mininet.link.Intf):
        """
        TapBridgeIntf is a Linux TAP interface, which is bridged with an NS-3 NetDevice.
        """
        def __init__ (self, name=None, node=None, port=None, ueGwIpAddr=None, ueIp=None,
                      localNode=None, csock=None, **params):
            self.name = name
            self.node = node
            self.ueGwIpAddr = ueGwIpAddr
            self.ueIp = ueIp
            self.localNode = localNode
            self.csock = csock
            self.createTap (self.name)
            self.delayedMove = True
            if node.inNamespace == True:
                self.inRightNamespace = False
            else:
                self.inRightNamespace = True
            mininet.link.Intf.__init__ (self, name, node, port, **params)

            self.csock.sendall ('nsDevice = ueLteDev.Get (0)\n')

            self.csock.sendall ('tapBridgeHelper = TapBridgeHelper ()\n')
            self.csock.sendall ('tapBridgeHelper.SetAttribute ("Mode", StringValue ("ConfigureLocal"))\n')
            self.csock.sendall ('tapBridgeHelper.SetAttribute ("DeviceName", StringValue ("{0}"))\n'.format (self.name))
            self.csock.sendall ('macAddress = Mac48Address.Allocate ()\n')
            self.csock.sendall ('tapBridgeHelper.SetAttribute ("MacAddress", Mac48AddressValue (macAddress))\n')
            self.csock.sendall ('tb = tapBridgeHelper.Install (nsNode, nsDevice)\n')
            self.csock.sendall ('tapBridges.append (tb)\n')

            self.csock.sendall ('dev = nsDevice.GetObject (LteUeNetDevice.GetTypeId ())\n')
            self.csock.sendall ('dev.SetMacAddress (macAddress)\n')
            self.csock.sendall ('dev.SetGatewayMacAddress (gatewayMacAddr)\n')

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
            self.node.cmd ('ip addr add dev {0} {1}/8'.format (self.name, self.ueIp))
            self.node.cmd ('ip route add default via {0}'.format (self.ueGwIpAddr))
            self.node.cmd ('arp -s {0} 00:00:00:00:00:00'.format (self.ueGwIpAddr))
            pat = '[0-9]*\.'
            route = (re.findall (pat, self.ueIp))[0] + '0.0.0'
            self.node.cmd ('ip route del {0}/8'.format (route))

        def cmd (self, *args, **kwargs):
            if self.inRightNamespace == True:
                return self.node.cmd (*args, **kwargs)
            else:
                return self.localNode.cmd (*args, **kwargs)

        def createTap (self, name):
            self.node.cmd ('ip tuntap add {0} mode tap'.format (name))
            self.node.cmd ('ip link set dev {0} netns 1'.format (name))
