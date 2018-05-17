// This block will be better on the parser declaration
// but piarser don't accept ifs, so I have to put it here
// sometimes p4 is a pain in the ass

action set_downlink() {
  modify_field(abstract_packet.up_down, PKT_DOWNLINK);
}

action set_uplink(){
  modify_field(abstract_packet.up_down, PKT_UPLINK);
}

table process_identification {
  reads {
    abstract_packet.in_port : exact;
  }
  actions {
    set_downlink;
    set_uplink;
  }
  size: TABLES_INGRESS_PACKET_TYPE_IDENTIFICATION;
}

