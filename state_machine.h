#ifndef _STATE_MACHINE_H_
#define _STATE_MACHINE_H_

#include <pthread.h>

#define DEBUG_INFO

#define WEATHER_INFO_BYTE 0x00
#define ANGLE_INFO_BYTE 0x01
#define BUFF_SIZE 1024

#define ANGLE_HEADER 0X68
#define ANGLE_LENGTH 0X0D
#define WEATHER_HEADER1 0XFF
#define WEATHER_HEADER2 0X20
#define WEATHER_END 0XAA

typedef enum {
	STATE_ANGLE_READY = 0,
	STATE_ANGLE_HEADER,
	STATE_ANGLE_LENGTH,
	STATE_ANGLE_CHECK,
} STATE;

typedef enum {
	STATE_WEATHER_READY = 0,
	STATE_WEATHER_HEADER1,
	STATE_WEATHER_HEADER2,
	STATE_WEATHER_END,
} STATE_2;

struct buffer
{
	unsigned char ring_buffer[BUFF_SIZE];
	pthread_mutex_t lock;
	int push, pop;
	pthread_cond_t buffer_notempty;
	pthread_cond_t buffer_notfull;
};

//int state_machine_fun(unsigned char data, unsigned char *frame_pointer, unsigned char *frame_info);
int state_machine_fun(unsigned char data, unsigned char *frame_info);
int state_machine_fun_2(unsigned char data, unsigned char *frame_info);
void init_buff(struct buffer *space);
//void choose_serial_com(unsigned char *device, unsigned char *com_number);

ssize_t tread(int fd, void *buf, size_t nbytes, unsigned int timout);
ssize_t treadn(int fd, void *buf, size_t nbytes, unsigned int timout);

#endif
