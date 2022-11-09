#!/usr/bin/python
import datetime
import time
import can

can.rc['interface'] = 'socketcan_ctypes'
from can.interfaces.interface import Bus
from can import Message

def check_rx(id,data):
     now = datetime.datetime.now()
     timeString = now.strftime("%d.%m.%Y %H:%M:%S ")
     print timeString," ID ",id," Data",data

def main():
        can_interface = 'can0'
        bus = Bus(can_interface)

        try:
           while True:
             Message = bus.recv(0.0)
             if Message:
                #check_rx(Message.arbitration_id, Message.data[0:8])
		#print "extended",Message.extended_id
		print "is_remote",Message.is_remote_frame
		print "type",Message.id_type
		print "is_error",Message.is_error_frame
		print "arb_id",Message.arbitration_id
		print "dlc",Message.dlc
		print "data",Message.data

        except  KeyboardInterrupt:
                bus.shutdown()

if __name__ == "__main__":
        main()
