action _drop() {
  drop();
}

action forward_packet(port) {
  modify_field(standard_metadata.egress_spec, port);
}

action forward_controller(){
  modify_field(abstract_packet._table, TABLE_FORWARD_ID);
  modify_field(abstract_packet.to_control, 1);
  modify_field(standard_metadata.egress_spec, 3);
}

table process_forwarding {
  reads {
    abstract_packet.in_port: exact;
    abstract_packet.dev_id: exact;
  }
  actions {
    forward_packet;
    forward_controller;
    _drop;
  }
  size: TALBES_EGRESS_FORWARDING;
}
