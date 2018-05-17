class P4PktOut(object):

  def __init__(self):
    self.fake_in_port = 0
    self.tech = 0
    self.up_down = 0

  def setFakeInPort(self, in_port):
    self.fake_in_port = in_port

  def setTechnology(self, tech):
    self.tech = tech

  def setUpDown(self, up_down):
    self.up_down = up_down

  def to_string(self):
    print "fake_in_port: " + str(self.fake_in_port)
