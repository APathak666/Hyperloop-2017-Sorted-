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


#define PORT 8080
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
void sCComm(int fd)
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
     	printf("%x",buffer[0]);
     	
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


int createTCPSocket()
{    //creating a server side socket 
	int fd=socket(AF_INET,SOCK_STREAM,0);
	//storing the details of the server in addr 
	struct sockaddr_in addr;
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr =INADDR_ANY;
	addr.sin_port = htons(PORT);
	if(connect(fd,(struct sockaddr*) &addr,sizeof(addr))>=0)
		printf("Client side connection estd.\n");
	return fd;

}












int main(int argc , char* argv[])
{
	int client;
	//calling func for creating socket and assigning its fd to fdQueue 
	client=createTCPSocket();
	
    printf("connection estd.");
    sCComm(client);
    close(client);
    printf("connection terminated\n");
    return 0;
}


