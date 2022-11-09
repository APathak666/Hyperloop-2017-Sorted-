#!/usr/bin/python
"""
Functionlity Description:
     Receive and Send data froma  can bus
     Creates a socket that will connect with a server and transmit the following Data

Variables that need to changed:
     "remote_ip" to the ip address of the base station laptop

     send_messages_can

     rcv_messgaes_socket

     pod_data_packet_can

     pod_data_packet_socket

Initial Tests:

1.  Run this code with all the dependencies installed and then with "remote_ip" as 127.0.0.1 run the file
    server_for_py.js and open index.html in browser and see if the data packet no is changing
"""

import datetime
import time
import can
import socket   #for sockets
import sys  #for exit
import json

can.rc['interface'] = 'socketcan_ctypes'


from can.interfaces.interface import Bus
from can import Message


# Dict to set the bits for the respective command
# Need to test it with Teensy Network and update the data formats
send_messages_can = {"Start": "00000000","Stop": "00000000", "Emergency Stop":"00000000",
                    "Engage Brakes":"00000000", "Disengage Brakes":"00000000",
                    "Engage Clutch":"00000000", "Disengage Clutch":"00000000",
                    "Actuate Levitation Module":"00000000","Unactuate Levitation Module":"00000000",
                     "Engage Auxillary Propulsion":"00000000","Disengage Auxillary Propulsion":"00000000" }

#List of commands that can be sent by the base station
rcv_messgaes_socket = ["Start","Stop","Emergency Stop","Engage Brakes","Disengage Brakes","Engage Clutch", 
                    "Disengage Clutch","Actuate Levitation Module","Unactuate Levitation Module",
                    "Engage Auxillary Propulsion","Disengage Auxillary Propulsion"]

#Dict to update the data received from the can network
#Created to have different freqs of transmission between (comm node,base) and (comm node, can network)
#Log this if needed
pod_data_packet_can = {"team_id":0,"stat":0,"acceleration":0,"position":0,"velocity":0,"battery_voltage":0,
                    "battery_current":0,"battery_temperature":0,"pod_temperature":0,"stripe_count":0}


#Dict to update the data that needs to be send to the Base Station backend
#Created to have different freqs of transmission between (comm node,base) and (comm node, can network)
pod_data_packet_socket = {"data_packet_no":0,"team_id":0,"stat":0,"acceleration":0,"position":0,"velocity":0,"battery_voltage":0,
                    "battery_current":0,"battery_temperature":0,"pod_temperature":0,"stripe_count":0}

#Valid ids on the CAN bus
valid_ids = [001,005,009,010,011,012,013,014,00A,00B,00C]
#data frame id info for different nodes
nodes_data_frames_id = {"power_node":"","nav_node":[001,005,009,010,011,012,013,014,00A,00B,00C],"control_node":""}
#emergency frame id info for different nodes
node_emergency_frames_id = {"power_node":"","nav_node":"","control_node":""}

#test = {'a':123,'b':231}

comm_node_header ={}

data_rcv = None;

#Set the ip of the base station when deployed
#Use the local ip in the network
remote_ip = "127.0.0.1"#"192.168.0.51"
host = 'Base Station';
port = 3000;#Port for TCP comm

def can_rcv(Message):#To handle the data processing from the canbus
    #Recevie the can packet and decode the node origin and call update functions
    ########## Example Case ##########
    """ (NOT Tested)
    If node A with id (arbitration id) sends the following data "00112233"
    
    "Message.arbitration_id" will give the id value
    "Message.data[0:8]" will give the data, check encoding format 
    
    Depending on these two values update the "pod_data_packet_can" as needed:
        Suppose 33 corresponds to position value
        pod_data_packet_can["position"] = Message.data[6:8]

    You can either do this in this function or list all the cases in update_can function
    """

    now = datetime.datetime.now()
    timeString = now.strftime("%d.%m.%Y %H:%M:%S")

    #Timestamp for logging on BeagleBone
    #Write logging code

    ##print "extended",Message.extended_id
    data_rcv = Message.data[0:8] # The 8 byte data recieved from the can bus
    message_id = Message.arbitration_id
    if(message_id in valid_ids):
        update_canData(data_rcv,message_id)
    else:
        print "Invalid Message ID"

    ##print "is_remote",Message.is_remote_frame
    ##print "type",Message.id_type
    ##print "is_error",Message.is_error_frame
    ##print "arb_id",Message.arbitration_id
    ##print "dlc",Message.dlc
    


    print "data",Message.data[0:8]
    print "Can Messgae Received!"
    
    return Message.data[0:8]

def can_send(reply):
    #Set the can packet to send the data on the canbus that has been received from the  base station server/socket

    print "Data received from socket",reply
    
    # Parameters that were working with Teensy CAN bus (Tested)
    reply_data = reply[1:len(reply)]
    Message.extended_id = False
    Message.is_remote_frame = False
    Message.id_type = False
    Message.is_error_frame = False
    Message.arbitration_id = 000 #Change Accordingly, id of the data packet onto the canbus
    Message.dlc = 8 #Change Accordingly, Message Length onto can 
    Message.data = bytearray([1, 2, 3])

    return Message

def update_canData(data,msg_id):
    #update data in pod_data_packet_can according to Header frame
    print""

    if()

def update_socketData():
    #update data in pod_data_packet_socket according to Header frame
    print""


def string_out(data):
    buff =""
    for key,value in data.items():
        buff = buff + str(value)

    return buff

def main():

    can_interface = 'can0' #Defining the can interface 
    canbus_handler = Bus(can_interface) # creating a handle for interacting with the can bus
    
    conn_status = 111 #connection status for the socket connection with the Base station
    
    try:# To establish the connection with node server via a TCP socket using python socket with remote ip and port no
        print "Trying to connect to Server"
        
        socket_handle = socket.socket(socket.AF_INET, socket.SOCK_STREAM)#creates a python TCP socket

        """
        socket_handle = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        socket_handle.bind((remote_ip, port))
        socket_handle.listen(1)
        conn, addr = socket_handle.accept()
        """

        while(conn_status!=0): # To wait for the connection to take place

            conn_status = socket_handle.connect_ex((remote_ip , port))
            # conn_status = 111 -> Not connected
            # conn_status = 0 -> Connection Established

        if(conn_status == 0):
            print "Connected to the Base Station Server"
            #print conn_status
    except socket.error:
        print 'Failed to create socket'
        #sys.exit()

    
    #message = "Ping from pod!! \n" #Test message
    #no_recv_packets = 1; 
    #print "#"
    send_flag = False # Flag to be set when there is data to be sent onto the can bus
    #data =bytearray([1, 2, 3]);
    #print send_messages
    no_packets_sent =0;

    try: 
        while True:

            message_can_rcv = canbus_handler.recv(0.0) # Listening to the CAN Bus and store any message in message_can_rcv 

            if message_can_rcv:# If any message
                can_out = can_rcv(message_can_rcv) #Send the message to can_rcv function to handle data processing
                #socket_handle.sendall(str(pod_data_packet_socket))
            

            pod_data_packet_socket['data_packet_no'] = pod_data_packet_socket['data_packet_no'] +1;
            json_pod_data = json.dumps(pod_data_packet_socket) #Conversion to json data for data retrieval at Node js server
            #json_pod_data = json.loads(json_pod_data)

            socket_handle.sendall(json_pod_data)# Sending the json_data_pod to the socket
            
            reply = socket_handle.recv(4096) # Receives data from the socket i.e Blocking mode for socket
            
            no_packets_sent = no_packets_sent+1;

            if(len(reply)>0 and reply[0]!= 'N'):
                print reply #Data received from the base server/socket
                send_flag = True

            if(send_flag):
                Msg = can_send(reply) #If any message from the Base server send to can_send to format the message
                canbus_handler.send(Msg) # Send the formatted CAN message
                send_flag = False

    except  KeyboardInterrupt:
        canbus_handler.shutdown()

    except socket.error:
        print "Socket Send/Receive Error"



if __name__ == "__main__":
    main()
