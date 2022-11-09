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

#define QUEUE_SIZE 5
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


int createTCPSocket()
{    //creating a server side socket 
	int fd=socket(AF_INET,SOCK_STREAM,0);
	//storing the details of the server in addr 
	struct sockaddr_in addr;
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr =INADDR_ANY;
	addr.sin_port = htons(PORT);
	//binding the server socket to addr ;its corresp fd is server
	bind(fd,(struct sockaddr*) &addr, sizeof addr);
	//listening to client req to connect 
	if(listen(fd,1)>=0)
		return fd;


}




void sCComm(int fd)
{	
	 do {
   do {
recv(fd, buffer,34 , 0);
         
            team_id=*((uint8_t*)buffer);
           
         status=*((uint8_t*)(buffer+1));
         acceleration=*((int32_t*)(buffer+2));
       	position=*((int32_t*)(buffer+6));
     	velocity=*((int32_t*)(buffer+10));
     	battery_voltage=*((int32_t*)(buffer+14));
     	battery_current=*((int32_t*)(buffer+18));
     	battery_temperature=*((int32_t*)(buffer+22));
     	pod_temperature=*((int32_t*)(buffer+26));
     	stripe_count=*((int32_t*)(buffer+30));

           	printf("id:%d",team_id );
           	printf("\nstatus:%d",status);
            printf("\nacceleration:%d",acceleration);
            printf("\nposition:%d",position);
            printf("\nvelocity:%d",velocity);
            printf("\nbattery_voltage:%d",battery_voltage);
            printf("\nbattery_current:%d",battery_current);
            printf("\nbattery_temperature:%d",battery_temperature);
            printf("\npod_temperature:%d",pod_temperature);
            printf("\nstripe_count:%d",stripe_count);
        } while (1);
       

  
        

} while (1);



}




void dispatch(int fd)
{
	
	int server=accept(fd,NULL,NULL);
		printf("connection established\n");
		sCComm(server);
	

}






int main(int argc , char* argv[])
{
	int fdQueue;
	//calling func for creating socket and assigning its fd to fdQueue 
	fdQueue=createTCPSocket();
    dispatch(fdQueue);
    close(fdQueue);
    printf("connection terminated\n");
    return 0;
}



