import runtime_CLI
import argparse
import cmd
import os
import sys
import struct
import json
import bmpy_utils as utils

from sswitch_runtime import SimpleSwitch
from scapy.all import *
from functools import wraps
import bmpy_utils as utils

from bm_runtime.standard import Standard
from bm_runtime.standard.ttypes import *
try:
    from bm_runtime.simple_pre import SimplePre
except:
    pass
try:
    from bm_runtime.simple_pre_lag import SimplePreLAG
except:
    pass

class SimpleSwitchAPI(runtime_CLI.RuntimeAPI):
    @staticmethod
    def get_thrift_services():
        return [("simple_switch", SimpleSwitch.Client)]

    def __init__(self, pre_type, standard_client, mc_client, sswitch_client):
        self.cli = runtime_CLI.RuntimeAPI.__init__(self, pre_type,
                                        standard_client, mc_client)
        self.sswitch_client = sswitch_client

    def do_set_queue_depth(self, line):
        "Set depth of one / all egress queue(s): set_queue_depth <nb_pkts> [<egress_port>]"
        args = line.split()
        depth = int(args[0])
        if len(args) > 1:
            port = int(args[1])
            self.sswitch_client.set_egress_queue_depth(port, depth)
        else:
            self.sswitch_client.set_all_egress_queue_depths(depth)

    def do_set_queue_rate(self, line):
        "Set rate of one / all egress queue(s): set_queue_rate <rate_pps> [<egress_port>]"
        args = line.split()
        rate = int(args[0])
        if len(args) > 1:
            port = int(args[1])
            self.sswitch_client.set_egress_queue_rate(port, rate)
        else:
            self.sswitch_client.set_all_egress_queue_rates(rate)

    def do_mirroring_add(self, line):
        "Add mirroring mapping: mirroring_add <mirror_id> <egress_port>"
        args = line.split()
        mirror_id, egress_port = int(args[0]), int(args[1])
        self.sswitch_client.mirroring_mapping_add(mirror_id, egress_port)

    def do_mirroring_delete(self, line):
        "Delete mirroring mapping: mirroring_delete <mirror_id>"
        mirror_id = int(line)
        self.sswitch_client.mirroring_mapping_delete(mirror_id)

    def do_get_time_elapsed(self, line):
        "Get time elapsed (in microseconds) since the switch started: get_time_elapsed"
        print self.sswitch_client.get_time_elapsed_us()

    def do_get_time_since_epoch(self, line):
        "Get time elapsed (in microseconds) since the switch clock's epoch: get_time_since_epoch"
        print self.sswitch_client.get_time_since_epoch_us()

