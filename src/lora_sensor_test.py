from scapy.all import *
from lora_packet import *

if __name__ == '__main__':
  lora = LoraPacket()
  frame = lora.make_packet(0x07, 0x14236598, 0x12, 0x00, 0x11, "Teste")
  hexdump(frame)
  

