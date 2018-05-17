action change_delay(delay){
  modify_field(abstract_packet.rx_win, delay);
}

action rx_win_controller(){
  add_header(packet_in);
  modify_field(packet_in.pkt_type, TABLE_RX_DELAY);
  modify_field(packet_in.dev_id, abstract_packet.dev_id);
  modify_field(packet_in.tech_id, abstract_packet.tech_id);
  modify_field(packet_in.in_port, abstract_packet.in_port);
  modify_field(packet_in.rx_win, 0);
  modify_field(standard_metadata.egress_spec, 3);
}

table process_rx_win {
  reads {
    abstract_packet.dev_id : exact;
  }
  actions {
    change_delay;
    rx_win_controller;
  }
  size: TABLES_INGRESS_RX_WIN;
}

