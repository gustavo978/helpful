header_type abstract_packet_t {
  fields {
    _table:       8;
    to_control:   8;
    dev_id:       48;
    app_id:       8;
    tech_id:      8;
    in_port:      8;
    up_down:      8;
    rx_win:       8;
    tx_channel:   8;
    remove_out:   8;
  }
}


// Header definition for FCtrl Downlink
//header_type down_fctrl_t {
//  fields {
//    down_adr:     1;
//    down_rfu:     1;
//    down_ack:     1;
//    down_fpeding: 1;
//    down_fopts:   1;
//  }
//}

// Header Definition for FCtrl Uplink
//header_type up_fctrl_t {
//  fields {
//    up_adr:       1;
//    up_adrackreq: 1;
//    up_down_ack:  1;
//    up_rfu:       1;
//    up_fopts:     1;   
//  }
//}

