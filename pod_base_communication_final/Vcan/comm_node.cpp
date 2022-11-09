#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <net/if.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <syslog.h>

#include <netdb.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>

#define MAX_SIZE 50



// All data fields definitions
// Currently set to int32_t

int32_t Team_Id ;
int32_t Status ;

int32_t Nav_Acceleration ;
int32_t Nav_Yaw;
int32_t Nav_Pitch;
int32_t Nav_Roll;
int32_t Nav_Position;
int32_t Nav_Velocity;
int32_t Nav_LTS_Brake_1_2;
int32_t Nav_LTS_Brake_3_4;
int32_t Nav_RR_Strip_Count;


int32_t PWR_Voltage;
int32_t PWR_Current;
int32_t PWR_Temperature;


int32_t CTRL_Temperature;
int32_t CTRL_Pressure;
int32_t CTRL_LTS_Height_1_2;
int32_t CTRL_LTS_Height_3_4;

//SEND BUFFER
char buffer[72];// To pack pod data to be sent to the base station 

void *connection_handler(void *socket_desc);

void update_packet(int message_id,int data);

int main()
{       


    long thread_num =1;
    pthread_t socket_base_thread;
    
        if( pthread_create( &socket_base_thread , NULL ,  connection_handler , (void *) thread_num) < 0)
        {
            perror("could not create thread");
            return 1;
        }

    int can_socket_handler;
    int can_rcv;
    struct sockaddr_can addr;
    struct can_frame frame;
    struct ifreq ifr;

    const char *ifname = "vcan0";// Interface name that the CAN is operating on

    if((can_socket_handler = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
            perror("Error while opening socket");
            return -1;
    }

    strcpy(ifr.ifr_name, ifname);
    ioctl(can_socket_handler, SIOCGIFINDEX, &ifr);

    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    printf("%s at index %d\n", ifname, ifr.ifr_ifindex);

    if(bind(can_socket_handler, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("Error in socket bind");
            return -2;
    }

    
    int payload_data = 0;
    //char *buffer_ptr = &buffer;

    while(1)
    {
            can_rcv = read(can_socket_handler, &frame, sizeof(struct can_frame));

            if (can_rcv < 0) {
                perror("can raw socket read");
                return 1;
            }

            if (can_rcv < sizeof(struct can_frame)) {
                fprintf(stderr, "read: incomplete CAN frame\n");
                return 1;
            }
            
            printf("Payload Data Length = %d  bytes \n", frame.can_dlc);
            //printf("%d\n",frame.data[0]);

            payload_data = frame.data[0];

            update_packet(frame.can_id,payload_data);
            
            //recv_packet(Base_Station_Socket_handler);
    }

    
    
    pthread_exit(NULL);
    return 0;

}

void *connection_handler(void *threadid)
{
    long threadnum = (long) threadid;
    int base_Station_Socket_handler;
    struct sockaddr_in serv_addr;
    char rcv_buffer[MAX_SIZE];
    char *ret;
    char ch = 'Z';
    int start = 1;

    if((base_Station_Socket_handler = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        printf("Failed creating socket\n");

    bzero((char *) &serv_addr, sizeof (serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(3000);

    

    while(1){

        //int cn_d = connect(Base_Station_Socket_handler,(struct sockaddr*) &addr1,sizeof(addr1));
        int conn_status = connect(base_Station_Socket_handler,(struct sockaddr*) &serv_addr,sizeof(serv_addr));
        if(conn_status>=0){
            printf("Client Side connection established!!\n");
            break;
        }
    }

    //printf("Connected successfully client:%ld\n", threadnum);
    while(1)
    {

        send(base_Station_Socket_handler,buffer,72,0);


        if(recv(base_Station_Socket_handler,rcv_buffer,MAX_SIZE,0)==0)
            printf("Error in Base Station receive buffer");
        else if(rcv_buffer[0] == 'y' || rcv_buffer[0] == 'Y'){

                
                ret = strchr(rcv_buffer,ch);
                printf("Command Received from Base Station:\n");
                printf("%.*s\n", (int)(ret-rcv_buffer-1), rcv_buffer+start);
                
            }
           
        usleep(1000);
    }

    close(base_Station_Socket_handler);
    return 0;
}

void update_packet(int message_id,int data){

         Team_Id = 1;
         Status =1;

         Nav_Acceleration =1;
         Nav_Yaw= 2;
         Nav_Pitch= 3;
         Nav_Roll= 4;
         Nav_Position= 5;
         Nav_Velocity= 6;
         Nav_LTS_Brake_1_2=7;
         Nav_LTS_Brake_3_4=8;
         Nav_RR_Strip_Count=9;

         PWR_Voltage = 10;
         PWR_Current = 11;
         PWR_Temperature = 12;
         
         CTRL_Temperature = 13;
         CTRL_Pressure = 14;
         CTRL_LTS_Height_1_2 = 15;
         CTRL_LTS_Height_3_4 = 16;


        
        printf("Read ID = %d\n",message_id);

        *((int32_t*)(buffer))=Team_Id;
        *((int32_t*)(buffer+4))=Status;

        if(message_id == 161){
            // Message id = A1 ;Nav
            printf("Received CAN Broadcast 1 from Navigation Node \n");
            *((int32_t*)(buffer+8))=Nav_Acceleration;
            *((int32_t*)(buffer+12))=Nav_Yaw;
            *((int32_t*)(buffer+16))=Nav_Pitch;
            *((int32_t*)(buffer+20))=Nav_Roll;
        }
        else if(message_id == 162){
            // Message id = A2 ;Nav
            printf("Received CAN Broadcast 2 from Navigation Node \n");
            *((int32_t*)(buffer+24))=Nav_Position;
            *((int32_t*)(buffer+28))=Nav_Velocity;
        }
        else if(message_id == 163){
            // Message id = A3 ;Nav
            printf("Received CAN Broadcast 3 from Navigation Node \n");
            *((int32_t*)(buffer+32))=Nav_LTS_Brake_1_2;
        }
        else if(message_id == 164){
            // Message id = A4 ;Nav
            printf("Received CAN Broadcast 4 from Navigation Node \n");
            *((int32_t*)(buffer+36))=Nav_LTS_Brake_3_4;
        }
        else if(message_id == 165){
            // Message id = A5 ;Nav
            printf("Received CAN Broadcast 5 from Navigation Node \n");
            *((int32_t*)(buffer+40))=Nav_RR_Strip_Count;
        }
        else if(message_id == 193){
            // Message id = C1; Power
            printf("Received CAN Broadcast 1 from Power Node \n");
            *((int32_t*)(buffer+44))=PWR_Voltage;
        }
        else if(message_id == 194){
            // Message id = C2; Power
            printf("Received CAN Broadcast 2 from Power Node \n");
            *((int32_t*)(buffer+48))=PWR_Current;
        }
        else if(message_id == 195){
            // Message id = C3; Power
            printf("Received CAN Broadcast 3 from Power Node \n");
            *((int32_t*)(buffer+52))=PWR_Temperature;
        }
        else if(message_id == 225){
            // Message id = E1; Control
            printf("Received CAN Broadcast 1 from Control Node \n");
            *((int32_t*)(buffer+56))=CTRL_Temperature;
            *((int32_t*)(buffer+60))=CTRL_Pressure;
        }
        else if(message_id == 226){
            // Message id = E2; Control
            printf("Received CAN Broadcast 2 from Control Node \n");
            *((int32_t*)(buffer+64))=CTRL_LTS_Height_1_2;
        }
        else if(message_id == 227){
            // Message id = E3; Control
            printf("Received CAN Broadcast 3 from Control Node \n");
            *((int32_t*)(buffer+68))=CTRL_LTS_Height_3_4;
        }

        return;
}

