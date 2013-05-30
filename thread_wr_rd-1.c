#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include "state_machine.h"

//#define BUFF_SIZE 1024


//struct buffer
//{
//	unsigned char ring_buffer[BUFF_SIZE];
//	pthread_mutex_t lock;
//	int push, pop;
//	pthread_cond_t buffer_notempty;
//	pthread_cond_t buffer_notfull;
//};

extern struct buffer *buff_ring;
extern struct buffer *buff_ring_2;
extern pthread_mutex_t udp_lock; 
//extern pthread_cond_t start_udp;

//extern pthread_mutex_t com_lock;
//extern pthread_cond_t com_ready;
extern int socket_fd;
extern struct sockaddr_in *udp_arm;

unsigned char data_udp[5];
STATE current_state;
STATE_2 current_state_2;

void init_buff(struct buffer *space)
{
	printf("struct buffer init start!\r\n");
	
	pthread_mutex_init(&space->lock, NULL);
	pthread_cond_init(&space->buffer_notempty, NULL);
	pthread_cond_init(&space->buffer_notfull, NULL);
	space->push = 0;
	space->pop = 0;

	printf("struct buffer init end!\r\n");
}

void push_back(struct buffer *space, unsigned char data)
{
	pthread_mutex_lock(&space->lock);
	if((space->push + 1) % BUFF_SIZE == space->pop) {
		pthread_cond_wait(&space->buffer_notfull, &space->lock);
	}
	//memcpy(space->ring_buffer + space->push, data, num);
	space->ring_buffer[space->push] = data;
	space->push = (space->push + 1) % BUFF_SIZE;
	
	pthread_cond_signal(&space->buffer_notempty);
	pthread_mutex_unlock(&space->lock);
	printf("push_back one times over\n");
}

unsigned char pop_front(struct buffer *space)
{
	unsigned char data;
	int i;
	unsigned char sum = 0;

	pthread_mutex_lock(&space->lock);
	if(space->push == space-> pop) {
		pthread_cond_wait(&space->buffer_notempty, &space->lock);
	}

	data = space->ring_buffer[space->pop];
	space->pop = (space->pop+1) % BUFF_SIZE;
	pthread_cond_signal(&space->buffer_notfull);
	pthread_mutex_unlock(&space->lock);
	printf("pop_front one times over\n");
	return data;
}

void *thread_push_back_fun(void *fd_com)
{
	unsigned char recv_buffer[14];
	int recv_bytes;
	int port_fd;
	int i;

	port_fd = *(int *)fd_com;
	printf("com_port is %d\r\n", port_fd);

	memset(recv_buffer, 0, 14);

	printf("thread_push_back_fun starting!\r\n");

//	buff_ring = (struct buffer *)malloc(sizeof(struct buffer));
//	init_buff(buff_ring);

	while(1)
	{
		//recv_bytes = read(port_fd, recv_buffer, 14);
		//recv_bytes = readport(port_fd, recv_buffer, 14, 500);
		//pthread_mutex_lock(&com_lock);
		//pthread_cond_wait(&com_ready, &com_lock);
		//usleep(200000);
		recv_bytes = treadn(port_fd, recv_buffer, 14, 10000);
		//recv_bytes = treadn(port_fd, recv_buffer, 14, 2000);

		if(recv_bytes > 0)
		{
			#ifdef DEBUG_INFO
				for(i=0; i<14; i++)
				{
					printf("__0x%.2x",recv_buffer[i]);
				}
				printf("recv_bytes = %d\n",recv_bytes);
			#else
			#endif
			//memcpy(push, recv_buffer, recv_bytes);
			for(i=0; i< recv_bytes; i++)
			{
				push_back(buff_ring, recv_buffer[i]);
			}
		}
		else ;
		    //printf("did not recev bytes!\n");
		//else printf("recv_bytes = 0\n");
		memset(recv_buffer, 0, 14);
		//pthread_mutex_unlock(&com_lock);
		select_sleep(0,10000);
	}
//	free(buff_ring);
//	buff_ring = NULL;
}


void *thread_push_back_fun_2(void *fd_com)
{
	unsigned char recv_buffer_2[7];
	int recv_bytes;
	int port_fd;
	int i;

	port_fd = *(int *)fd_com;
	printf("com_port_2 is %d\r\n", port_fd);

	memset(recv_buffer_2, 0, 7);

	printf("thread_push_back_fun_2 starting!\r\n");

//	buff_ring = (struct buffer *)malloc(sizeof(struct buffer));
//	init_buff(buff_ring);

	while(1)
	{
		//recv_bytes = read(port_fd, recv_buffer, 14);
		//recv_bytes = readport(port_fd, recv_buffer, 14, 500);
		recv_bytes = treadn(port_fd, recv_buffer_2, 7, 10000);
		//recv_bytes = treadn(port_fd, recv_buffer, 14, 2000);

		if(recv_bytes > 0)
		{
			#ifdef DEBUG_INFO
				for(i=0; i<7; i++)
				{
					printf("--0x%.2x",recv_buffer_2[i]);
				}
				printf("recv_bytes_2 = %d\n",recv_bytes);
			#else
			#endif
			//memcpy(push, recv_buffer, recv_bytes);
			for(i=0; i< recv_bytes; i++)
			{
				push_back(buff_ring_2, recv_buffer_2[i]);
			}
		}
		else ; 
			//printf("no recv_bytes\n");
		memset(recv_buffer_2, 0, 7);
		select_sleep(0,10000);
	}
//	free(buff_ring);
//	buff_ring = NULL;
}


void *thread_pop_front_fun()
{
	int pop_data;
	unsigned char *data_recv;
	current_state = STATE_ANGLE_READY;
	int i;
	int catch_frame_flag = 0;
	int upload_data_flag = 0;
	unsigned char frame_data[12];
	unsigned int angle_value;
	int flag;
	
	printf("thread_pop_front_fun starting!\r\n");
	data_recv = NULL;
	data_recv = (unsigned char *)malloc(5*sizeof(char));
	while(1)
	{
		//valid_data = NULL;
		//valid_data = (unsigned char *)malloc(5*sizeof(char));
		pop_data = pop_front(buff_ring);
		printf("*pop_data is 0x%.2x\n", pop_data);
		
		catch_frame_flag = state_machine_fun(pop_data, frame_data);
		
		if(current_state == STATE_ANGLE_LENGTH)
		{
			frame_data[i] = pop_data;
			i++;
			if(i > 11){
				current_state = STATE_ANGLE_CHECK;
				i = 0;
			}
		}
		else {
			i = 0;
			//memset(frame_data, 0, 11);
		}
		
		//printf("catch_frame_flag is %d \n", catch_frame_flag);
		//if(valid_data != NULL) {
		if (catch_frame_flag == 2)
		{
			printf("catch_frame_flag is %d \n", catch_frame_flag);
			*data_recv = ANGLE_INFO_BYTE;

        	angle_value = (*(frame_data+3)&0x0F)*10000+((*(frame_data+4)&0xF0)>>4)*1000+(*(frame_data+4)&0x0F)*100+((*(frame_data+5)&0xF0)>>4)*10+(*(frame_data+5)&0x0F);
    		*(data_recv+1) = angle_value/256;
    		*(data_recv+2) = angle_value%256;
    		flag = *(frame_data+3) >> 4;
    		if(flag) {
    			*(data_recv+1) += 0x80;
    		}
    		else ;
			flag = 0;
			angle_value = 0;
    		angle_value = (*(frame_data+6)&0x0F)*10000+((*(frame_data+7)&0xF0)>>4)*1000+(*(frame_data+7)&0x0F)*100+((*(frame_data+8)&0xF0)>>4)*10+(*(frame_data+8)&0x0F);
    		*(data_recv+3) = angle_value/256;
    		*(data_recv+4) = angle_value%256;
    		flag = *(frame_data+6) >> 4;
    		if(flag) {
    			*(data_recv+3) += 0x80;
    		}
    		else ;
    		memset(frame_data, 0 , 12);
    		upload_data_flag = 1;
		}
		else {
			upload_data_flag = 0;
			memset(data_recv, 0, 5);
		}

		//pthread_mutex_lock(&udp_lock);
		if(upload_data_flag)
		{
			//for(i=0; i<5; i++)
			//{
			//	printf("__0x%.2x",*(data_recv+i));
			//}
			//i = 0;
			upload_data_flag = 0;
			pthread_mutex_lock(&udp_lock);
			//memcpy(data_udp, data_recv, 5);
			//pthread_cond_signal(&start_udp);
			upload_data_fun(data_recv, socket_fd, udp_arm);
			memset(data_recv, 0, 5);
			printf("angle info uploaded\n");
			pthread_mutex_unlock(&udp_lock);
		}
		select_sleep(0,10000); 
		//pthread_mutex_unlock(&udp_lock);
		//free(valid_data);
		//valid_data = NULL;
	}
	free(data_recv);
	data_recv = NULL;
	//free(valid_data);
}



void *thread_pop_front_fun_2()
{
	int pop_data;
	unsigned char *data_recv;
	current_state_2 = STATE_WEATHER_READY;
	int i;
	int catch_frame_flag = 0;
	int upload_data_flag = 0;
	unsigned char frame_data[5];
	unsigned int wind_speed;
	unsigned int cal_h;
	unsigned int cal_l;
	unsigned int wind_direction;
	unsigned int x1,x2,x3,x4;
	
	printf("thread_pop_front_fun_2 starting!\r\n");
	data_recv = NULL;
	data_recv = (unsigned char *)malloc(5*sizeof(char));
	while(1)
	{
		//valid_data = NULL;
		//valid_data = (unsigned char *)malloc(5*sizeof(char));
		pop_data = pop_front(buff_ring_2);
		printf("*pop_data_2 is 0x%.2x\n", pop_data);
		
		catch_frame_flag = state_machine_fun_2(pop_data, frame_data);
		
		if(current_state_2 == STATE_WEATHER_HEADER2)
		{
			frame_data[i] = pop_data;
			i++;
			if(i > 4) {
				current_state_2 = STATE_WEATHER_END;
				i = 0;
			}
		}
		else {
			i = 0;
			//memset(frame_data, 0, 11);
		}
		
		//printf("catch_frame_flag is %d \n", catch_frame_flag);
		//if(valid_data != NULL) {
		if (catch_frame_flag == 1)
		{
			printf("catch_frame_flag is %d \n", catch_frame_flag);
			*data_recv = WEATHER_INFO_BYTE;
			//memcpy(data_recv+1, frame_data+1, 4);
		
			x1 = ((*(frame_data+2)>>4)&0x0F)*1000;
			x2 = ((*(frame_data+2))&0x0F)*100;
			x3 = ((*(frame_data+1)>>4)&0x0F)*10;
			x4 = (*(frame_data+1))&0x0F ;
			wind_speed = x1+x2+x3+x4;
			x1 = 0;
			x2 = 0;
			x3 = 0;
			x4 = 0;
			printf("********************wind_speed is %d\n",wind_speed);
			if(wind_speed < 2560) {
				*(data_recv+1) = 0x00;
			}
			else {
				*(data_recv+1) = 0x01;
				wind_speed -= 2560;
			}
			cal_h = (wind_speed/160);
			cal_l = ceil((float)(wind_speed%160)/10);
			*(data_recv+2) = (cal_h<<4)+cal_l;

			wind_direction = (*(frame_data+3)<<8) + *(frame_data+4);
			if( 0xFEFF == wind_direction) {
            	*(data_recv+3) = 0x00;
            	*(data_recv+4) = 0x00;
			}
			else if( 0xFEFE == wind_direction) {
            	*(data_recv+3) = 0x00;
            	*(data_recv+4) = 0x0B;
			}
			else if( 0xFFFE == wind_direction) {;
				
            	*(data_recv+3) = 0x00;
            	*(data_recv+4) = 0x17;
			}
			else if( 0xFFFC == wind_direction) {
            	*(data_recv+3) = 0x00;
            	*(data_recv+4) = 0x22;
				
			}
			else if( 0xFFFD == wind_direction) {
            	*(data_recv+3) = 0x00;
            	*(data_recv+4) = 0x2D;
				
			}
			else if( 0xFFF9 == wind_direction) {
            	*(data_recv+3) = 0x00;
            	*(data_recv+4) = 0x38;
				
			}
			else if( 0xFFFB == wind_direction) {
            	*(data_recv+3) = 0x00;
            	*(data_recv+4) = 0x44;
				
			}
			else if( 0xFFF3 == wind_direction) {
            	*(data_recv+3) = 0x00;
            	*(data_recv+4) = 0x4F;
				
			}
			else if( 0xFFF7 == wind_direction) {
            	*(data_recv+3) = 0x00;
            	*(data_recv+4) = 0x5A;
				
			}
			else if( 0xFFE7 == wind_direction) {
            	*(data_recv+3) = 0x00;
            	*(data_recv+4) = 0x65;
				
			}
			else if( 0xFFEF == wind_direction) {
            	*(data_recv+3) = 0x00;
            	*(data_recv+4) = 0x71;
				
			}
			else if( 0xFFCF == wind_direction) {
            	*(data_recv+3) = 0x00;
            	*(data_recv+4) = 0x7C;
				
			}
			else if( 0xFFDF == wind_direction) {
            	*(data_recv+3) = 0x00;
            	*(data_recv+4) = 0x87;
				
			}
			else if( 0xFF9F == wind_direction) {
            	*(data_recv+3) = 0x00;
            	*(data_recv+4) = 0x92;
				
			}
			else if( 0xFFBF == wind_direction) {
            	*(data_recv+3) = 0x00;
            	*(data_recv+4) = 0x9E;
				
			}
			else if( 0xFF3F == wind_direction) {
            	*(data_recv+3) = 0x00;
            	*(data_recv+4) = 0xA9;
				
			}
			else if( 0xFF7F == wind_direction) {
            	*(data_recv+3) = 0x00;
            	*(data_recv+4) = 0xB4;
				
			}
			else if( 0x7F7F == wind_direction) {
            	*(data_recv+3) = 0x00;
            	*(data_recv+4) = 0xBF;
				
			}
			else if( 0x7FFF == wind_direction) {
            	*(data_recv+3) = 0x00;
            	*(data_recv+4) = 0xCB;
				
			}
			else if( 0x3FFF == wind_direction) {
            	*(data_recv+3) = 0x00;
            	*(data_recv+4) = 0xD6;
				
			}
			else if( 0xBFFF == wind_direction) {
            	*(data_recv+3) = 0x00;
            	*(data_recv+4) = 0xE1;
				
			}
			else if( 0x9FFF == wind_direction) {
            	*(data_recv+3) = 0x00;
            	*(data_recv+4) = 0xEC;
				
			}
			else if( 0xDFFF == wind_direction) {
            	*(data_recv+3) = 0x00;
            	*(data_recv+4) = 0xF8;
				
			}
			else if( 0xCFFF == wind_direction) {
            	*(data_recv+3) = 0x01;
            	*(data_recv+4) = 0x03;
				
			}
			else if( 0xEFFF == wind_direction) {
            	*(data_recv+3) = 0x01;
            	*(data_recv+4) = 0x0E;
				
			}
			else if( 0xE7FF == wind_direction) {
            	*(data_recv+3) = 0x01;
            	*(data_recv+4) = 0x19;
				
			}
			else if( 0xF7FF == wind_direction) {
            	*(data_recv+3) = 0x01;
            	*(data_recv+4) = 0x25;
				
			}
			else if( 0xF3FF == wind_direction) {
            	*(data_recv+3) = 0x01;
            	*(data_recv+4) = 0x30;
				
			}
			else if( 0xFBFF == wind_direction) {
            	*(data_recv+3) = 0x01;
            	*(data_recv+4) = 0x3B;
				
			}
			else if( 0xF9FF == wind_direction) {
            	*(data_recv+3) = 0x01;
            	*(data_recv+4) = 0x46;
				
			}
			else if( 0xFDFF == wind_direction) {
            	*(data_recv+3) = 0x01;
            	*(data_recv+4) = 0x52;
				
			}
			else if( 0xFCFF == wind_direction) {
            	*(data_recv+3) = 0x00;
            	*(data_recv+4) = 0x5D;
				
			}
			else {
				*(data_recv+3) = 0x00;
				*(data_recv+4) = 0x00;
			}
			//printf("_________________\n");
			//for(i=0; i<5; i++)
			//{
			//	printf("__0x%.2x",*(data_recv+i));
			//}
			//i = 0;
			//printf("_________________\n");
			memset(frame_data, 0 , 5);
			upload_data_flag = 1;
		}
		else {
			upload_data_flag = 0;
			memset(data_recv, 0, 5);
		}

		//pthread_mutex_lock(&udp_lock);
		if(upload_data_flag)
		{
			//for(i=0; i<5; i++)
			//{
			//	printf("__0x%.2x",*(data_recv+i));
			//}
			//i = 0;
			upload_data_flag = 0;
			pthread_mutex_lock(&udp_lock);
			//memcpy(data_udp, data_recv, 5);
			//pthread_cond_signal(&start_udp);
			upload_data_fun(data_recv, socket_fd, udp_arm);
			memset(data_recv, 0, 5);
			printf("weather info uploaded\n");
			pthread_mutex_unlock(&udp_lock);
		}
		select_sleep(0,10000); 
		//pthread_mutex_unlock(&udp_lock);
		//free(valid_data);
		//valid_data = NULL;
	}
	free(data_recv);
	data_recv = NULL;
}



ssize_t
tread(int fd, void *buf, size_t nbytes, unsigned int timout)
{
    int nfds;
    fd_set readfds;
    struct timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec = timout;
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    nfds = select(fd+1, &readfds, NULL, NULL, &tv);
    if(nfds <= 0) {
        if(nfds == 0)
            errno = ETIME;
        return(-1);
    }
    return(read(fd, buf, nbytes));
}

ssize_t
treadn(int fd, void *buf, size_t nbytes, unsigned int timout)
{
    size_t nleft;
    ssize_t nread;

    nleft = nbytes;
    while(nleft > 0) {
        if((nread = tread(fd, buf, nleft, timout)) < 0) {
            if(nleft == nbytes)
                return(-1);
            else
                break;
        }
        else if(nread == 0) {
            break;
        }
        nleft -= nread;
        buf += nread;
    }
    return(nbytes-nleft);
}
