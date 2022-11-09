import can

can.rc['interface'] = 'socketcan_ctypes'

from can.interfaces.interface import Bus
from can import Message
 
def main():
   can_interface = 'can0'
   bus = Bus(can_interface)

 
   print "Send a message..."
   Message.extended_id = True
   Message.is_remote_frame = False
   Message.id_type = 1
   Message.is_error_frame = False
   Message.arbitration_id = 0x00E07123
   Message.dlc = 1
   Message.data = [ 0x01]
   try:
       bus.send(Message);
   except:
       print "Ups something went wrong!"
 
if __name__ == "__main__":
   main()
