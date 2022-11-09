#Socket client example in python
 
import socket   #for sockets
import sys  #for exit
from 
 
#create an INET, STREAMing socket
try:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
except socket.error:
    print 'Failed to create socket'
    #sys.exit()
     
print 'Socket Created'
 
host = 'Base Station';
port = 3000;
 
try:
    remote_ip = "192.168.0.51"#127.0.0.1"#socket.gethostbyname( host )
 
except socket.gaierror:
    #could not resolve
    print 'Hostname could not be resolved. Exiting'
    sys.exit()
 
#Connect to remote server
s.connect((remote_ip , port))
 
print 'Socket Connected to ' + host + ' on ip ' + remote_ip
 
#Send some data to remote server
message = "Ping from pod!! \n"
no_recv_packets = 1;
while(True):
    try :
        #Set the whole string
        s.sendall(message)
        reply = s.recv(4096)

        print no_recv_packets
        no_recv_packets = no_recv_packets +1;

 
        print reply
    except socket.error:
        #Send failed
        print 'Send failed'
        sys.exit()
 
print 'Message send successfully'
 
#Now receive data
reply = s.recv(4096)
 
print reply
