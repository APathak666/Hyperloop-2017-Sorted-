import can

can.rc['interface'] = 'socketcan_ctypes'


from can.interfaces.interface import Bus
from can import Message

def main():
   can_interface = 'can0'
   bus = Bus(can_interface)
	

   print "Send a message..."
   #Check the current message params
   Message.extended_id = False
   Message.is_remote_frame = False
   Message.id_type = False
   Message.is_error_frame = False
   Message.arbitration_id = 001
   Message.dlc = 8
   Message.data = bytearray([1, 2, 3])
  
   print Message.data

   try:
    bus.send(Message);
   except:
       print "Ups something went wrong!"

if __name__ == "__main__":
   main()
