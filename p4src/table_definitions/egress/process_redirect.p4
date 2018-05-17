action do_cpu_encap() {
    add_header(packet_in);
    modify_field(packet_in.pkt_type,   abstract_packet._table);
    modify_field(packet_in.dev_id,     abstract_packet.dev_id);
    modify_field(packet_in.app_id,     abstract_packet.app_id);
    modify_field(packet_in.tech_id,    abstract_packet.tech_id);
    modify_field(packet_in.in_port,    abstract_packet.in_port);
    modify_field(packet_in.up_down,    abstract_packet.up_down);
    modify_field(packet_in.rx_win,     abstract_packet.rx_win);
    modify_field(packet_in.tx_channel, abstract_packet.tx_channel);
}

table process_redirect {
    reads { 
      abstract_packet.to_control : exact; 
    }
    actions { 
      _drop; 
      do_cpu_encap;
    }
    size : 16;
}

