#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <inttypes.h>


#define PORT 3000


uint8_t team_id;
uint8_t status;
int32_t acceleration;
int32_t position;
int32_t velocity;
int32_t battery_voltage;
int32_t battery_current;
int32_t battery_temperature;
int32_t pod_temperature;
uint32_t stripe_count;

char buffer[34];
char a[50];
int no_recv_data_packets = 0;

// Task 1: Modify this function to recieve data from the CAN bus.OR write a new one.

/*void sCComm(int fd) 
{   
         do {
           printf("Client: ");
        do { printf("\n team_id: ");
             scanf("%"SCNu8,&team_id);       

        
            
             printf("\n status: ");
             scanf("%"SCNu8 ,&status);
             
             printf("\n acceleration: ");
             scanf("%d" ,&acceleration);
             
             printf("\n position: ");
             scanf("%d" ,&position);
             
             printf("\n velocity: ");
             scanf("%d" ,&velocity);
             
             printf("\n battery_voltage: ");
             scanf("%d" ,&battery_voltage);
             
             printf("\n battery_current: ");
             scanf("%d" ,&battery_current);
             
             printf("\n battery_temperature: ");
             scanf("%d" ,&battery_temperature);
             
             printf("\n pod_temperature: ");
             scanf("%d" ,&pod_temperature);
             
             printf("\n stripe_count: ");
             scanf("%d" ,&stripe_count);


         

        *((uint8_t*)buffer)=team_id;
        
        *(uint8_t*)(buffer+1)=status;
        //printf("%x",buffer[0]);
        
        *((int32_t*)(buffer+2))=acceleration;
        *((int32_t*)(buffer+6))=position;
        *((int32_t*)(buffer+10))=velocity;
        *((int32_t*)(buffer+14))=battery_voltage;
        *((int32_t*)(buffer+18))=battery_current;
        *((int32_t*)(buffer+22))=battery_temperature;
        *((int32_t*)(buffer+26))=pod_temperature;
        *((int32_t*)(buffer+30))=stripe_count;
        
        


        send(fd, buffer,34, 0);


            
        } while (1); 

    } while (1);

}
*/

// Task 2:Modify this Function to send data received from the CAN bus to the socket created, format the data if required,etc 
void sendPacket_pod(int fd){
       
        //while(true) { 

         team_id = 1;
         status =1;
         acceleration =1;
         position =1;
         velocity =1;
         battery_voltage = 5;
         battery_current = 10;
         battery_temperature = 11;
         pod_temperature =10;
         stripe_count =10;


        *((uint8_t*)buffer)=team_id;
        
        *(uint8_t*)(buffer+1)=status;
        //printf("%x",buffer[0]);
        
        *((int32_t*)(buffer+2))=acceleration;
        *((int32_t*)(buffer+6))=position;
        *((int32_t*)(buffer+10))=velocity;
        *((int32_t*)(buffer+14))=battery_voltage;
        *((int32_t*)(buffer+18))=battery_current;
        *((int32_t*)(buffer+22))=battery_temperature;
        *((int32_t*)(buffer+26))=pod_temperature;
        *((int32_t*)(buffer+30))=stripe_count;
        
        


        send(fd, buffer,34, 0);
        //recv(fd, a,sizeof(a),0);

        //printf("%s",a);

            
        //} 

}


// Task 3: Receive Data from the node.js server and send data to the CAN bus.
void recvPacket_pod(int fd){

    //printf("#\n");

    recv(fd, a,sizeof(a),0);
    //printf("%s\n",a);

    if(a[0] == 'y' || a[0] == 'Y'){
         int start = 1;
         int len = 49;

        printf("%.*s\n", len, a + start);
        //printf("%s\n",a);
        printf("%d\n",(++no_recv_data_packets));
    }
    a==" ";

    //a = " ";
}



/*int createTCPSocket()
{    //creating a server side socket 
    int fd=socket(AF_INET,SOCK_STREAM,0);
    //int fd=socket(AF_INET,SOCK_DGRAM,0);
    //storing the details of the server in addr 
    /*struct sockaddr_in addr;
    
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr =inet_addr("127.0.0.1");
    addr.sin_port = htons(PORT);
    if(connect(fd,(struct sockaddr*) &addr,sizeof(addr))>=0)
        printf("Client side connection estd.\n");

    printf("%d",fd); 
    return fd;

}*/





int main()
{
    int client;
    //calling func for creating socket and assigning its fd to fdQueue 
    //client=createTCPSocket();
    client =socket(AF_INET,SOCK_STREAM,0);


    int cnt = 0;
    struct sockaddr_in addr1;
    
    addr1.sin_family = AF_INET;
    addr1.sin_addr.s_addr =inet_addr("127.0.0.1");
    addr1.sin_port = htons(PORT);

    /*int bind_out = bind(client, (struct sockaddr*) &addr1, sizeof(addr1));
    int no_of_connections = 5;
    int listen_out =  listen(client,no_of_connections);
    printf("%d\n",listen_out );
    

    for (;;)
        {
        // Fetch a new connection off the queue.
        int acc = accept(client,NULL,NULL);
        if (acc == -1)
        {
        // Oh dear! Something’s gone wrong! Whimper and die.
        perror("Could not accept incoming connection.");
        exit(EXIT_FAILURE);
        }
        // Got a connection! Handle it.
        printf("Client side connection estd!!!//!.\n");
        }*/

    //printf("%d\n",acc );
    //printf("%d\n",listen_out);
    //printf("connection estd.\n");
    //sCComm(client);
    printf("Listening For connections\n");
    while(true){
        //client=createTCPSocket();
        //printf("%d\n", ++cnt);
        
        int cn_d = connect(client,(struct sockaddr*) &addr1,sizeof(addr1));
        //printf("%d\n",cn_d);
        /*if(){
        printf("Client side connection estd!!!!!!!!!.\n");
        }*/
        if(cn_d>=0){
            printf("Client side connection estd!!!!!!!!!.\n");

            while(true){

            sendPacket_pod(client);
            //printf("#\n");
            recvPacket_pod(client);
            
            }
        }

    }

    //close(client);
    printf("connection terminated\n");
    return 0;
}


