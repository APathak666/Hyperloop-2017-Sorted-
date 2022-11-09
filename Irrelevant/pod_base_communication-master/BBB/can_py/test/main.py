#!/usr/bin/python
import datetime
import time
import can
import socket   #for sockets
import sys  #for exit

can.rc['interface'] = 'socketcan_ctypes'


from can.interfaces.interface import Bus
from can import Message

send_messages = {"Start": "00000000","Stop": "00000000", "Emergency Stop":"00000000",
                    "Engage Brakes":"00000000", "Disengage Brakes":"00000000",
                    "Engage Clutch":"00000000", "Disengage Clutch":"00000000",
                    "Actuate Levitation Module":"00000000","Unactuate Levitation Module":"00000000",
                     "Engage Auxillary Propulsion":"00000000","Disengage Auxillary Propulsion":"00000000" }

def can_rcv(Message):
    now = datetime.datetime.now()
    timeString = now.strftime("%d.%m.%Y %H:%M:%S")

    #print "extended",Message.extended_id
    print "is_remote",Message.is_remote_frame
    print "type",Message.id_type
    print "is_error",Message.is_error_frame
    print "arb_id",Message.arbitration_id
    print "dlc",Message.dlc
    print "data",Message.data[0:8]
    print "Can Messgae Received!"

def can_send():
    
    Message.extended_id = False
    Message.is_remote_frame = False
    Message.id_type = False
    Message.is_error_frame = False
    Message.arbitration_id = 001
    Message.dlc = 8
    Message.data = bytearray([1, 2, 3])

    return Message




def main():
    can_interface = 'can0'
    bus = Bus(can_interface)

    send_flag = False
    data =bytearray([1, 2, 3]);
    print send_messages
    
    try:
        while True:
            Message_rcv = bus.recv(0.0)
            if Message:
                can_rcv(Message_rcv)
            if(send_flag):
                Msg = can_send(data)
                bus.send(Message)

    except  KeyboardInterrupt:
        bus.shutdown()



if __name__ == "__main__":
    main()