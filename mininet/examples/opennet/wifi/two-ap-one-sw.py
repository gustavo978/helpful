#!/usr/bin/python

"""
+---------+ +---------+ +---------+
|   NS3   | | Mininet | |   NS3   |
|   AP0   +-+ Switch  +-+   AP1   |
+----+----+ +---------+ +----+----+
     |                       |
+----+----+             +----+----+
|  NS3    |             |  NS3    |
|  Station|             |  Station|
+---------+             +---------+
"""


from mininet.net import Mininet
from mininet.node import Node, Switch, RemoteController
from mininet.link import Link, Intf
from mininet.log import setLogLevel, info
from mininet.cli import CLI

import mininet.ns3
from mininet.ns3 import WIFISegment

import ns.core
import ns.network
import ns.wifi
import ns.csma
import ns.wimax
import ns.uan
import ns.netanim

from mininet.opennet import *

def main():

    net = Mininet()

    """ Uncomment following lines to add controller """
    # net.addController('c0', controller=RemoteController, ip="127.0.0.1", port=6633)

    sw0 = net.addSwitch('sw0', ip=None, failMode='standalone')
    ap0 = net.addSwitch('ap0', ip=None, failMode='standalone')
    ap1 = net.addSwitch('ap1', ip=None, failMode='standalone')
    sta0 = net.addHost('sta0', ip="10.0.0.1")
    sta1 = net.addHost('sta1', ip="10.0.0.2")

    wifi = WIFISegment ()
    wifi.addAp(ap0, channelNumber=11, ssid="opennet_0")
    wifi.addAp(ap1, channelNumber=11, ssid="opennet_1")

    wifi.addSta(sta0, channelNumber=11, ssid="opennet_0")
    wifi.addSta(sta1, channelNumber=11, ssid="opennet_1")

    net.addLink(sw0, ap0)
    net.addLink(sw0, ap1)

    net.start()
    mininet.ns3.start()

    sta0.cmdPrint('ping -c2 ' + sta1.IP())
    sta1.cmdPrint('ping -c2 ' + sta0.IP())

    CLI(net)

    mininet.ns3.stop()
    mininet.ns3.clear()
    net.stop()

if __name__ == '__main__':
    setLogLevel('info')
    main()
