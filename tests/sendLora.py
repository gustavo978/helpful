import socket

s = socket.socket(socket.AF_PACKET, socket.SOCK_RAW)
s.bind(("eth0",0))

ethernet_hdr = [0x01, 
                0x88, 0x14, 0x15, 0x11, 
                0x23,
                0x00, 0x90,
                0x2b, 0x14, 0x88, 0x14, 0x15, 0x11, 0x2b, 0x14, 0x88, 0x14, 0x15, 0x11
                ]
               
#ethernet_data_str = "Hello"
ethernet_hdr_str = "".join(map(chr, ethernet_hdr))
s.send(ethernet_hdr_str)
