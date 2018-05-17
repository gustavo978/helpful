// Header Lora
header_type lora_t {
  fields {
    mhdr:     8;
    devAddr:  32;
    FCtrl:    8;
    FCnt:     16;
    FOpts:    96; 
  }
  //length:     0;
  //max_length: 120;
}

header_type nb_iot_t {
  fields {
    to_do: 8;
  } 
}

header_type packet_in_t {
  fields {
    pkt_type:     8;     // byte 0
    dev_id:       48;    // byte 1 to 6 
    app_id:       8;     // byte 7
    tech_id:      8;     // byte 8
    in_port:      8;     // byte 9 
    up_down:      8;     // byte 10
    rx_win:       8;     // byte 11
    tx_channel:   8;     // byte 12
 }
}

header_type packet_out_t {
  fields {
    in_port: 8;
    tech_id: 8;
    up_down: 8;
  }
}
