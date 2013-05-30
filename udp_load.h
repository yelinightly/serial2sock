#ifndef _UDP_LOAD_H_
#define _UDP_LOAD_H_

#define START_BYTE 0x24
#define UDP_FRAME_LENGTH 0x0D //data length in udp frame is 13
#define COMMAND_BYTE 0x81
#define ANGLE_BYTE 0x01
#define WEATHER_BYTE 0x00
#define END_BYTE 0x23

#define BUREAU_ADDR 0x01
#define PLANT_ADDR 0x01
#define TOWER_HIGH_ADDR 0x00
#define TOWER_LOW_ADDR 0x01

int Init_Socket(char *IP, struct sockaddr_in *udp_sockaddr, int netport);
void select_sleep(int i, int j);
#endif
