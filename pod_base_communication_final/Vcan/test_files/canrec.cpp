#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

int main(void)
{
        int s;
        int nbytes;
        struct sockaddr_can addr;
        struct can_frame frame;
        struct ifreq ifr;

        const char *ifname = "vcan0";

        if((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
                perror("Error while opening socket");
                return -1;
        }

        strcpy(ifr.ifr_name, ifname);
        ioctl(s, SIOCGIFINDEX, &ifr);

        addr.can_family  = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;

        printf("%s at index %d\n", ifname, ifr.ifr_ifindex);

        if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
                perror("Error in socket bind");
                return -2;
        }

        frame.can_id  = 0x123;
        frame.can_dlc = 2;
        frame.data[0] = 0x11;
        frame.data[1] = 0x22;



        while(1)
        {
                nbytes = read(s, &frame, sizeof(struct can_frame));

                if (nbytes < 0) {
                    perror("can raw socket read");
                    return 1;
                }

                if (nbytes < sizeof(struct can_frame)) {
                    fprintf(stderr, "read: incomplete CAN frame\n");
                    return 1;
                }
                printf("Read ID = %d\n",frame.can_id);
                printf("Payload Data Length = %d \n bytes", frame.can_dlc);
                printf("%d\n",frame.data[0]);
        }

        return 0;
}