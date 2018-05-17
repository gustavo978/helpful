action remove_control_reader(){
  remove_header()
}

table process_forwarding {
  reads {
    abstract_packet.in_port: exact;
  }
  actions {
    remove_control_reader;
  }
  size: 1;
}
