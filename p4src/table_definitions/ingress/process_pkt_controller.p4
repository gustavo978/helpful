action remove_header(){
  modify_field(abstract_packet.in_port, bk_header.bk_in_port);
  
}

table process_pkt_controller{
  actions {
    _remove_header;
  }
  size: 1;
}
