/*******************
* Parser
********************/
parser start {
  set_metadata(abstract_packet.in_port, standard_metadata.ingress_port);
  return select(standard_metadata.ingress_port) {
    4: parser_controller_header;
    5: ppacket_in;
    default: frame_identification;
  }
}

parser ppacket_in {
  extract (packet_in);
  return ingress;
}

parser parser_controller_header {
  extract(packet_out);
  set_metadata(abstract_packet.tech_id, packet_out.tech_id);
  set_metadata(abstract_packet.in_port, packet_out.in_port);
  set_metadata(abstract_packet.up_down, packet_out.up_down);
  set_metadata(abstract_packet.remove_out, 1);

  return select (packet_out.tech_id) {
    TECHNOLOGY_LORA: packet_out_lora;
    TECHNOLOGY_NBIOT: packet_out_nbiot;
    default: ingress;
  }
}

parser packet_out_lora {
  extract (lora);
  set_metadata(abstract_packet.dev_id,  lora.devAddr);
  return ingress;
}

parser packet_out_nbiot {
  //TODO
  extract (nb_iot);
  return ingress;
}

parser frame_identification {
  return select(abstract_packet.in_port) {
    1: parse_lora;    // lora Uplink
    2: parse_lora;    // lora downlink
    6: parse_nb_iot;
    7: parse_nb_iot;
    //4: parse_lora;    // from controller
    default: ingress;
  } 
}


parser parse_lora {
  extract(lora);
  set_metadata(abstract_packet.tech_id, TECHNOLOGY_LORA);
  set_metadata(abstract_packet.dev_id,  lora.devAddr);
  set_metadata(abstract_packet.up_down, 0);
  return ingress;
}

parser parse_nb_iot {
  //TODO
  extract(nb_iot);
  return ingress;
}

