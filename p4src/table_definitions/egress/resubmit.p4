action _resubmit(){
  //remove_header(controller_header);
  modify_field(abstract_packet.to_control, 0);
  recirculate(copy_to_cpu_fields);
}

table process_resubmit{
  actions {
    _resubmit;
  }
  size: 1;
}
