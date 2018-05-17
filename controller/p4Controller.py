import runtime_CLI
import json

from p4_log import *
from definitions import *
from p4_pkt_mod import *
from simple_switch_api import *
from process_packet_mod import *

from sswitch_runtime import SimpleSwitch
from scapy.all import *

global process

def process_packet(pkt):
    global process
    process.process_pkt_mod_out(pkt)

def main():
    # Init Log
    global process
    log = p4Logging()

    # Init the runtime API
    pre = runtime_CLI.PreType.SimplePreLAG
    services = runtime_CLI.RuntimeAPI.get_thrift_services(pre)
    services.extend(SimpleSwitchAPI.get_thrift_services())
    standard_client, mc_client, sswitch_client = runtime_CLI.thrift_connect(
      'localhost', 9090, services
    )
    runtime_CLI.load_json_config(standard_client, "")
    cli = SimpleSwitchAPI(pre, standard_client, mc_client, sswitch_client)
  
    # Add default rules on the switch
    process = ProcessPacketOut(cli, log)
    process.add_default_rules()
  
    #Start sniff
    sniff(iface = "s1-eth3", prn=process_packet, store=0)

if __name__ == '__main__':
    main()

