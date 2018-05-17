action change_channel(channel){
  modify_field(abstract_packet.tx_channel, channel);
}

action channel_controller(){
  add_header(packet_in);
  modify_field(packet_in.pkt_type, TABLE_CHANNEL_ID);
  modify_field(packet_in.dev_id, abstract_packet.dev_id);
  modify_field(packet_in.tech_id, abstract_packet.tech_id);
  modify_field(packet_in.in_port, abstract_packet.in_port);
  modify_field(packet_in.tx_channel, 0);
  modify_field(standard_metadata.egress_spec, 3);
}

table process_select_channel {
  reads {
    abstract_packet.dev_id : exact;
  }
  actions {
    change_channel;
    channel_controller;
  }
  size: TABLES_INGRESS_SELECT_CHANNEL;
}

