#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include "udp_load.h"


//extern int socket_fd;
//extern unsigned char data_udp[5];
extern pthread_mutex_t udp_lock;
//extern pthread_cond_t start_udp;
unsigned char udp_frame[16];

void upload_data_fun(unsigned char data[5], int net_fd, struct sockaddr_in *udpsockaddr)
{
	//unsigned char udp_frame[16];
	int i;
	int sent_bytes;

	*(udp_frame) = START_BYTE;
	*(udp_frame+1) = UDP_FRAME_LENGTH;
	*(udp_frame+2) = BUREAU_ADDR;
	*(udp_frame+3) = PLANT_ADDR;
	*(udp_frame+4) = TOWER_HIGH_ADDR; //high byte of tower address;
	*(udp_frame+5) = TOWER_LOW_ADDR; //low byte of tower address;
	*(udp_frame+6) = *data;    //weather frame or angle frame;
	*(udp_frame+7) = COMMAND_BYTE;
	memcpy(udp_frame+8, data+1, 4);
	*(udp_frame+12) = 0x00;
	*(udp_frame+13) = 0x00;
	*(udp_frame+14) = 0x00;
	for(i=0; i<6; i++) {
		*(udp_frame+14) += (*(udp_frame+8+i));
	}
	*(udp_frame+15) = END_BYTE;
    //for(i=0; i<16; i++)
    //{
    //	printf("_*_*_*_*\n");
    //	printf("****%.2x",*(udp_frame+i));
    //	printf("_*_*_*_*\n");
    //}

	//use sendto in udp transfer
	sent_bytes = sendto(net_fd, udp_frame, 16, 0, (struct sockaddr *)udpsockaddr, sizeof(struct sockaddr_in));

	memset(udp_frame, 0, 16);
	
	if(sent_bytes < 0) {
		strerror(errno);
		printf("errno is %d\n", errno);
	}
	printf("%d data uploaded!\n",sent_bytes);
}

//void *thread_udp_upload_fun(void *IP_addr)
//{
//	int socket_fd;
//  struct sockaddr_in *udp_arm;
//  char *IP_address;
//	printf("thread_udp_upload_fun starting!\r\n");
    
//    IP_address = (char *)IP_addr;
//    udp_arm = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
//    printf("IP_address is %s\n",IP_address );
//	socket_fd = Init_Socket(IP_address, udp_arm);
//	while(1)
//	{
//		pthread_mutex_lock(&udp_lock);
//		pthread_cond_wait(&start_udp, &udp_lock);
//		upload_data_fun(data_udp, socket_fd, udp_arm);
//		memset(data_udp, 0, 5);
//		pthread_mutex_unlock(&udp_lock);
//	}
	
//}

void select_sleep(int i, int j)
{
	struct timeval timeout;

	timeout.tv_sec = i;
	timeout.tv_usec = j;

	select(0, NULL, NULL, NULL, &timeout);
}
