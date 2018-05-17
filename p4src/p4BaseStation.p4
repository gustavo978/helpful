// Includes 
#include "includes/definitions.h"
#include "includes/tables_size.h"
#include "includes/headers.p4"
#include "includes/parser.p4"
#include "includes/field_list.p4"

// Define metadata variables for ingress and egress metadata
#include "includes/ingress_metadata.p4"

// Include tables
#include "table_definitions/egress_tables.p4"
#include "table_definitions/ingress_tables.p4"

metadata abstract_packet_t abstract_packet;  
header packet_in_t packet_in;
header packet_out_t packet_out;
header lora_t lora;
header nb_iot_t nb_iot;

control ingress {
  // Start to apply the controll tables
  if (abstract_packet.remove_out == 1){
    apply(process_remove_packet_out);
  } else {
    apply(process_identification);
  }

  if (abstract_packet.up_down == PKT_DOWNLINK){
    apply(process_rx_win);
    if (abstract_packet.tech_id == TECHNOLOGY_LORA) {
      apply(process_select_channel);
    }
  }
  apply(process_forwarding);
}

control egress {
  if (abstract_packet.to_control == 1){
    apply(process_redirect);
  }
}

