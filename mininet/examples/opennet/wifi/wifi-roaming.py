#!/usr/bin/python

"""
This example shows how to create a simulation with two roaming hosts and seven APs
"""

from mininet.net import Mininet
from mininet.node import Node, OVSSwitch, Host, RemoteController
from mininet.link import Link, Intf
from mininet.cli import CLI
from mininet.log import setLogLevel, info
from mininet.ns3 import WIFISegment

import mininet.ns3
import ns.core
import ns.network
import ns.wifi
import ns.wimax
import ns.uan
import ns.csma
import ns.netanim

from mininet.opennet import *

def WifiNet():

    net = Mininet()

    """ Uncomment following lines to add controller """
    # info( '*** Adding controller\n' )
    # net.addController( 'c0', controller=RemoteController, ip='127.0.0.1', port=6633 )

    """ Initialize WifiSegment
        Reference mininet/ns3.py for more detail of WIFISegment class """
    wifi = WIFISegment()

    """ Node names """
    hosts = ['h1', 'h2']
    switches = ['s1', 's2', 's3', 's4', 's5', 's6', 's7']

    """ Links between APs """
    links = [ 
              ['s1', 's2'], ['s1', 's3'], ['s1', 's4'],
              ['s1', 's5'], ['s1', 's6'], ['s1', 's7']
            ]

    """ IP address of hosts """
    ip = {'h1': '10.0.0.1', 'h2': '10.0.0.2'}

    """ Wi-Fi channels """
    channels = {
                'h1': 1 , 'h2': 11, 's1': 1 , 's2': 6 , 's3': 11,
                's4': 11, 's5': 6 , 's6': 11, 's7': 11
               }

    """ This rectangle is the bounds of mobility model """
    rectangle = ns.mobility.RectangleValue(ns.mobility.Rectangle(-150, 150, -150, 150))

    """ The mobility helpers of hosts """
    mobility_helpers = {'h1': mininet.ns3.createMobilityHelper("ns3::RandomWalk2dMobilityModel",
                                                               n0="Bounds", v0=rectangle
                                                              ),
                        'h2': mininet.ns3.createMobilityHelper("ns3::RandomWalk2dMobilityModel",
                                                               n0="Bounds", v0=rectangle
                                                              )
                       }

    list_position = {'h1': mininet.ns3.createListPositionAllocate(x1=0,y1=10,z1=0),
                     'h2': mininet.ns3.createListPositionAllocate(x1=150,y1=30,z1=0)
                    }

    """ Mobility models of hosts """
    mobility_models = {'h1': mininet.ns3.setListPositionAllocate(mobility_helpers['h1'],
                                                                 list_position['h1']
                                                                ),
                       'h2': mininet.ns3.setListPositionAllocate(mobility_helpers['h2'],
                                                                 list_position['h2']
                                                                )
                      }

    """ Positions of APs """
    positions = {'s1': (0, 0, 0), 's2': (120, 0, 0), 's3': (60, 60*(3**0.5), 0),
                 's4': (60, -60*(3**0.5), 0), 's5':  (-120, 0, 0),
                 's6': (-60, 60*(3**0.5), 0), 's7': (-60, -60*(3**0.5), 0)
                }

    """ Container of nodes """
    nodes = {}

    """ Initialize Stations """
    for host in hosts:
        node = net.addHost(host, ip=ip[host])
        mininet.ns3.setMobilityModel(node, mobility_models.get(host))
        wifi.addSta(node, channelNumber=channels.get(host), ssid='opennet')
        nodes[host] = node

    """ Initialize APs """
    for switch in switches:
        node = net.addSwitch(switch, ip=None, failMode='standalone')
        mininet.ns3.setMobilityModel(node, None)
        pos_tuple = positions.get(switch)
        mininet.ns3.setPosition(node, pos_tuple[0], pos_tuple[1], pos_tuple[2])
        wifi.addAp(node, channelNumber=channels.get(switch), ssid='opennet')
        nodes[switch] = node

    """ Add links between APs """
    for link in links:
        name1, name2 = link[0], link[1]
        node1, node2 = nodes[name1], nodes[name2]
        net.addLink(node1, node2)

    """ Enable Pcap output """
    pcap = Pcap()
    pcap.enable()
    print pcap

    """ Enable netanim output """
    anim = Netanim("/tmp/xml/wifi-wired-bridged4.xml", nodes)
    print anim

    """ Update node descriptions in the netanim """
    for key in nodes.keys():
        node = nodes[key]
        description = "{0}-{1}".format(str(node), str(node.nsNode.GetId()))
        anim.UpdateNodeDescription (node.nsNode, description)
        if isinstance(node, OVSSwitch):
            color = (0, 255, 0)
        elif isinstance(node, Host):
            color = (0, 0, 255)
        anim.UpdateNodeColor(node.nsNode, color[0], color[1], color[2])

    """ Start the simulation """
    info( '*** Starting network\n' )
    net.start()
    mininet.ns3.start()

    info( 'Testing network connectivity\n' )
    nodes['h1'].cmdPrint( 'ping 10.0.0.2 -c 3' )
    
    CLI( net )

    info( '*** Stopping network\n' )
    mininet.ns3.stop()
    info( '*** mininet.ns3.stop()\n' )
    mininet.ns3.clear()
    info( '*** mininet.ns3.clear()\n' )
    net.stop()
    info( '*** net.stop()\n' )

if __name__ == '__main__':
    setLogLevel( 'info' )
    WifiNet()

