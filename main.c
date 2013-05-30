/****************************
*
* project: SERIALCOM2UDP
* compile: make or arm-linux-gcc *.c -lpthread -lm -static -o target
* reboot system when the program crush and write info to tower_monitor_log
*
****************************/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include "thread_fun.h"
#include "state_machine.h"
#include "udp_load.h"


struct buffer *buff_ring;
struct buffer *buff_ring_2;

pthread_mutex_t udp_lock;
//pthread_cond_t start_udp;
//int serial_com_fd; no need for global variable

//pthread_mutex_t com_lock;
//pthread_cond_t com_ready;

int socket_fd;
struct sockaddr_in *udp_arm;

void *set_timer();
void signal_handler(int m);

int main(int argc,char* argv[])
{
    pid_t fork_process, wait_process;

    int com_fd;
    int com_fd_2;
    unsigned char *dev;
    unsigned char *dev_2;
    int written_bytes = 0;
	char *addr_IP;
	int sock_port;
	int log_fd;

	time_t timep;
	struct tm *tp;
	char time_now[23];
	char log_info[40];

	pthread_t th_push_back;
	pthread_t th_push_back_2;
	pthread_t th_pop_front;
	pthread_t th_pop_front_2;
	pthread_t th_query;
//	pthread_t th_udp_upload;

    printf("*****************************\n");
    printf("project SERIALCOM2UDP starting!\r\n");
    printf("*****************************\n");

	if(5 != argc) {
		printf("argument error, check serial com setting\n");
		printf("need to set like this >>>> ./app tty* tty* ip_address sock_port\n");
		return -1;
	}

    fork_process = fork();
    if(fork_process < 0 ){
    	printf("Error occured on forking!\n");
    }
    else if(fork_process > 0){
    	printf("parent process, waitting for ending of child process and reboot\n");
    	wait(NULL);
    	log_fd = open("/home/log_file", O_RDWR|O_CREAT|O_APPEND);
    	time(&timep);
    	tp = localtime(&timep);
    	memset(time_now, 0, 23);
    	memset(log_info, 0 , 40);
    	sprintf(time_now, "%d/%d/%d--%d:%d:%d", (1900+tp->tm_year),(1+tp->tm_mon),tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec);
    	memcpy(log_info, "program error!  ",16);
    	memcpy(log_info+16, time_now, sizeof(time_now));
    	log_info[39] = '\n'; 
    	write(log_fd, log_info, sizeof(log_info));
    	system("reboot");
    }
    else
    {
    	printf("child process, execute the program\n");
    
    //int com_fd;
    //int com_fd_2;
    //unsigned char *dev;
    //unsigned char *dev_2;
    //int written_bytes = 0;
	//char *addr_IP;

	//pthread_t th_push_back;
	//pthread_t th_push_back_2;
	//pthread_t th_pop_front;
	//pthread_t th_pop_front_2;
	//pthread_t th_query;
	//pthread_t th_udp_upload;
/*
    printf("*****************************\n");
    printf("project SERIALCOM2UDP starting!\r\n");
    printf("*****************************\n");

	if(5 != argc) {
		printf("argument error, check serial com setting\n");
		printf("need to set like this: ./app tty* tty* ip_address sock_port\n");
		return -1;
	} */

	dev = (unsigned char *)malloc(20*sizeof(unsigned char));
	dev_2 = (unsigned char *)malloc(20*sizeof(unsigned char));
	memcpy(dev, "/dev/", 5);
	memcpy(dev_2, "/dev/", 5);  
	memcpy(dev+5, argv[1], (int)strlen(argv[1]));
	memcpy(dev_2+5, argv[2], (int)strlen(argv[2]));
	printf("1st serial com is %s\n", dev);
	printf("2nd serial com is %s\n", dev_2);
	//choose_serial_com(dev, argv[1]);
	//choose_serial_com(dev_2, argv[2]);

	if(strcmp(dev,dev_2) == 0) {
		return -1;
	}	

	buff_ring = (struct buffer *)malloc(sizeof(struct buffer));
	init_buff(buff_ring);
	buff_ring_2 = (struct buffer *)malloc(sizeof(struct buffer));
	init_buff(buff_ring_2);

	pthread_mutex_init(&udp_lock, NULL);
//	pthread_cond_init(&start_udp, NULL);

//	pthread_mutex_init(&com_lock, NULL);
//	pthread_cond_init(&com_ready, NULL);

	com_fd = Init_SerialCom(dev, 9600);
	printf("com fd is %d\r\n", com_fd);
	com_fd_2 = Init_SerialCom(dev_2, 9600);
	printf("com fd_2 is %d\r\n", com_fd_2);

    addr_IP = argv[3]; 
    printf("determination address is %s\n", addr_IP);
	udp_arm = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
	sock_port = atoi(argv[4]);
    printf("udp port no. is %d\n", sock_port);

	socket_fd = Init_Socket(addr_IP, udp_arm, sock_port);


	//create thread
//	pthread_create(&th_udp_upload, NULL, thread_udp_upload_fun, addr_IP);

	pthread_create(&th_push_back, NULL, thread_push_back_fun, &com_fd);
	pthread_create(&th_pop_front, NULL, thread_pop_front_fun, NULL);

	pthread_create(&th_push_back_2, NULL, thread_push_back_fun_2, &com_fd_2);
	pthread_create(&th_pop_front_2, NULL, thread_pop_front_fun_2, NULL);
	
	pthread_create(&th_query, NULL, thread_query_fun, &com_fd);
//	pthread_create(&th_udp_upload, NULL, thread_udp_upload_fun, addr_IP);

	//wait for end of thread
	pthread_join(th_query, NULL);
	pthread_join(th_push_back, NULL);
	pthread_join(th_pop_front, NULL);
	
	pthread_join(th_push_back_2, NULL);
	pthread_join(th_pop_front_2, NULL);
	
//	pthread_join(th_udp_upload,NULL);

	printf("project over!\n");
	free(buff_ring);
	free(buff_ring_2);
	buff_ring = NULL;
	buff_ring_2 = NULL;
	free(dev);
	free(dev_2);
	dev = NULL;
	dev_2 = NULL;
	} //if(fork_process < 0)
}
