action remove() {
  remove_header(packet_out);
}

table process_remove_packet_out {
  reads {
    abstract_packet.remove_out : exact;
  }
  actions {
    remove;
  }
  size: TABLES_INGRESS_PACKET_TYPE_IDENTIFICATION;
}

