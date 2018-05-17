class P4PktIn(object):

  PKT_MOD_FORWARD = 0x1
  PKT_MOD_CHANNEL = 0x2
  PKT_MOD_RX_WIN  = 0x3

  def __init__(self, pkt):
    self.pkt_type   = int(pkt[0],16)   ## byte 0
    self.dev_id     = pkt[1:7]           ## byte 1 to 6
    self.app_id     = int(pkt[7],16)     ## byte 7
    self.tech_id    = int(pkt[8],16)    ## byte 8
    self.in_port    = int(pkt[9],16)    ## byte 9
    self.up_down    = int(pkt[10],16)   ## byte 10
    self.rx_win     = int(pkt[11],16)    ## byte 12
    self.tx_channel = int(pkt[12],16)
    self.str_dev_id = "0x"

    for i in range(0,len(self.dev_id)):
      self.str_dev_id += '{:02x}'.format(int(self.dev_id[i],16))
      if (i == (len(self.dev_id)-1)):
        break

  def to_string(self):
    print "pkt_type   " + str(self.pkt_type)
    print "dev_id "     + self.str_dev_id
    print "app_id     " + str(self.app_id)
    print "tech_id    " + str(self.tech_id)
    print "in_port    " + str(self.in_port)
    print "up_down    " + str(self.up_down)
    print "rx_win     " + str(self.rx_win)
    print "tx_channel " + str(self.tx_channel)

