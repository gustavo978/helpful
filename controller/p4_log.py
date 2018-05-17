from datetime import datetime
import logging as log


class p4Logging(object):
    def __init__(self, file_name = None):
        self.log = log
        date = datetime.now()
        str_date = date.strftime('%Y-%m-%d.%H:%M:%S')
        self.file_name = "log/p4Controler-" + str_date
        log.basicConfig(filename=self.file_name, level=log.INFO)
        
    def log_cmd(self, msg):
        self.log.info("CMD: " + msg)

    def log_info(self, msg):
        self.log.info("CMD: " + msg)

