import runtime_CLI
import json

from p4_log import *
from definitions import *
from p4_pkt_in import *
from p4_pkt_out import *
from simple_switch_api import *

from sswitch_runtime import SimpleSwitch
from scapy.all import *

class ProcessPacketOut(object):

    def __init__(self, cli, log):
        self.cli = cli    
        self.log = log
        self.default_rules = []
        self.channel_1 = []
        self.channel_2 = []
        self.channel_3 = []
        
        self.default_rules.append(P4_SET_DEFAULT + " "\
                + TABLE_FORWARDING + " "\
                + ACTION_FORWARD_CONTROLLER)

        self.default_rules.append(P4_SET_DEFAULT + " "\
                + TABLE_REMOVE_PACKET_OUT + " "\
                + ACTION_REMOVE)

        self.default_rules.append(P4_TABLE_ADD + " " \
                + TABLE_PKT_IDENTIFICATION + " "\
                + ACTION_SET_DOWNLINK + " " \
                + str(PKT_TYPE_DOWNLINK) + " "\
                + "=>")

        self.default_rules.append(P4_TABLE_ADD + " " \
                + TABLE_PKT_IDENTIFICATION + " "\
                + ACTION_SET_UPLINK + " " \
                + str(PKT_TYPE_UPLINK) + " "\
                + "=>")

        #self.default_rules.append(P4_SET_DEFAULT + " "\
                #+ TABLE_CHANNEL + " "\
                #+ ACTION_CHANNEL_CONTROLLER)

        self.default_rules.append(P4_TABLE_ADD + " "\
                + TABLE_REDIRECT + " "\
                + ACTION_DO_CPU_ENCAP + " "\
                + '1 => ')

    """
    Add default rules to the tables
    """
    def add_default_rules(self):
        for cmd in self.default_rules:
            self.cli.onecmd(cmd)
            self.log.log_cmd(cmd)


    """
    Control function for the Forwarding Table
    """
    def control_forward_table(self,p4):
        cmds    = []
        inport  = 0
        outport = 0
        p4_out = P4PktOut()

        self.log.log_info("InPort: " + str(p4.in_port))

        if p4.in_port == SWITCH_PORT_1:
            inport = SWITCH_PORT_1
            outport = SWITCH_PORT_2

        elif p4.in_port == SWITCH_PORT_2:
            inport = SWITCH_PORT_2
            outport = SWITCH_PORT_1

        else:
            self.log.log_info("Packet Unknow")

        cmds.append(P4_TABLE_ADD + " "\
            + TABLE_FORWARDING + " "\
            + ACTION_FORWARD_PACKET + " "\
            + str(inport) + " " \
            + p4.str_dev_id + " " \
            ' => ' + \
            str(outport))

        p4_out.setFakeInPort(inport)
        p4_out.setTechnology(p4.tech_id)
        p4_out.setUpDown(p4.up_down)

        for cmd in cmds:
            self.log.log_cmd(cmd)
            self.cli.onecmd(cmd)

        return p4_out

    """
    Control function for Channel Selection Table
    """
    def control_channel_table(self,p4):
        p4_out = P4PktOut()
        size_1 = len(self.channel_1)
        size_2 = len(self.channel_2)
        size_3 = len(self.channel_3)
        channel = 0

        #Select Channel based on the allocation
        if (size_1 < size_2) and (size_1 < size_3):
            channel = 1
        elif (size_2 < size_3):
            channel = 2
        else:
            channel = 3

        cmd = P4_TABLE_ADD + " "\
            + TABLE_SELECT_CHANNEL + " "\
            + ACTION_CHANGE_CHANNEL + " "\
            + p4.str_dev_id + " " \
            ' => ' + \
            str(channel)

        p4_out.setFakeInPort(p4.in_port)
        p4_out.setTechnology(p4.tech_id)
        p4_out.setUpDown(p4.up_down)

        self.send_packet_out(pkt_in, p4_out)
 
    """
    Control RX Table
    """
    def control_rx_table(self, p4):
      p4_out = P4PktOut()
      return p4_out

    """
    Global runtime CLI
    """
    def send_packet_out(self, pkt_in, p4_out):
        int_r = []
        relay = pkt_in[13:len(pkt_in)]
        int_r.append(p4_out.fake_in_port)
        int_r.append(p4_out.tech)
        int_r.append(p4_out.up_down)
        for r in relay:
            int_r.append(int(r,16))
        pkt_out = "".join(map(chr, int_r))

        s = socket.socket(socket.AF_PACKET, socket.SOCK_RAW)
        s.bind(("s1-eth4",0))
        s.send(pkt_out)

    """
    Process the packet mod and out
    """
    def process_pkt_mod_out(self, pkt):
        pkt_in = map(hex, map(ord, str(pkt)))
        p4_in = P4PktIn(pkt_in)
        p4_out = None

        if p4_in.pkt_type == P4PktIn.PKT_MOD_FORWARD:
            self.log.log_info("Packet in by forwarding")
            p4_out = self.control_forward_table(p4_in)

        elif p4_in.pkt_type == P4PktIn.PKT_MOD_CHANNEL:
            self.log.log_info("Packet in by channel")
            p4_out = self.control_channel_table(p4_in)

        elif p4_in.pkt_type == P4PktIn.PKT_MOD_RX_WIN:
            self.log.log_info("Packet in by Rx window")
            p4_out = self.control_rx_table(p4_in)

        else:
            self.log.log_info("Packet Unknow")
        
        self.send_packet_out(pkt_in, p4_out)

