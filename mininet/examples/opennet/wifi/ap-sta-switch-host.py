#!/usr/bin/python

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

    wifi = WIFISegment ()

    # About AP
    ap0 = net.addSwitch('ap0', failMode='standalone')
    wifi.addAp(ap0, channelNumber=6, ssid="opennet_ap")

    # About Station
    sta0 = net.addHost('sta0', ip='10.0.0.2')
    wifi.addSta(sta0, channelNumber=6, ssid="opennet_ap")

    # About OVSSwitch
    s0 = net.addSwitch('s0', failMode='standalone')

    h1 = net.addHost('h1', ip='10.0.0.1')

    net.addLink(s0, ap0)
    net.addLink(s0, h1)

    print("Switches list: {0}\nHosts list: {1}\n".format(net.switches, net.hosts))

    mininet.ns3.start()
    net.start()

    net.pingAll ()

    CLI(net)

    mininet.ns3.stop()
    net.stop()


if __name__ == '__main__':
    setLogLevel('info')
    main()
