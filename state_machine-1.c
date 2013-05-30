#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "state_machine.h"

extern current_state;
extern current_state_2;

int state_machine_fun(unsigned char data, unsigned char *frame_info)
{

	int i = 0;
	unsigned char sum = 0;	
    int flag_frame;
    flag_frame = 0;



	switch(current_state)
	{
		case STATE_ANGLE_READY:
			if(ANGLE_HEADER == data) {
				current_state = STATE_ANGLE_HEADER;
				printf("STATE_ANGLE_HEADER\n");
			}
			else {
				current_state = STATE_ANGLE_READY;
			}
			break;
		case STATE_ANGLE_HEADER:
			if(ANGLE_LENGTH == data) {
				current_state = STATE_ANGLE_LENGTH;
				printf("STATE_ANGLE_LENGTH\n");
			}
			else {
				current_state = STATE_ANGLE_READY;
			}
			break;
		case STATE_ANGLE_LENGTH:
			break;
		case STATE_ANGLE_CHECK:
			for(i=0;i<12;i++) {
				sum += *(frame_info + i);
			}
			printf("angle_sum is 0x%.2x\n", sum);
			if(sum == data) {
				current_state = STATE_ANGLE_READY;
				flag_frame = 2;
			}
			else {
				current_state = STATE_ANGLE_READY;
			}
			break;
		default: 
			current_state = STATE_ANGLE_READY;
			break;
	}
	return flag_frame;
}



int state_machine_fun_2(unsigned char data, unsigned char *frame_info)
{

	int i = 0;
	unsigned char sum = 0;	
    int flag_frame;
    flag_frame = 0;



	switch(current_state_2)
	{
		case STATE_WEATHER_READY:
			if(WEATHER_HEADER1 == data) {
				current_state_2 = STATE_WEATHER_HEADER1;
				printf("STATE_WEATHER_HEADER1\n");
			}
			else {
				current_state_2 = STATE_WEATHER_READY;
			}
			break;
		case STATE_WEATHER_HEADER1:
			if(WEATHER_HEADER2 == data) {
				current_state_2 = STATE_WEATHER_HEADER2;
				printf("STATE_WEATHER_HEADER2\n");
			}
			else {
				current_state_2 = STATE_WEATHER_READY;
			}
			break;
		case STATE_WEATHER_HEADER2:
			break;
		case STATE_WEATHER_END:
			//for(i=1;i<5;i++) {
			//	sum += *(frame_info + i);
			//}
			//printf("weather_sum is 0x%.2x\n", sum);
			if(WEATHER_END == data) {
				current_state_2 = STATE_WEATHER_READY;
				flag_frame = 1;
			}
			else {
				current_state_2 = STATE_WEATHER_READY;
			}
			break;
		default: 
			current_state_2 = STATE_WEATHER_READY;
			break;
	}
	return flag_frame;
}
