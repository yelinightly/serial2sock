#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>


//#define __SERIAL_COM__ "/dev/ttyS3"
//#define __BAUDRATE__ B9600

//#define __NET_PORT__ 5010
//const char *IP = "192.168.1.11";

//int serial_com_fd;
//int socket_fd;

int Init_SerialCom(unsigned char *com_device, unsigned int baudrate_value)
{
	struct termios oldtio, newtio;
    //extern serial_com_fd;
    int serial_port_fd;
    int baud_val;

	//serial_port_fd= open(__SERIAL_COM__,O_RDWR|O_NOCTTY|O_NONBLOCK);
    serial_port_fd= open(com_device, O_RDWR|O_NOCTTY|O_NDELAY|O_NONBLOCK);
	if(serial_port_fd < 0){
		//perror(__SERIAL_COM__);
        perror(com_device);
		return(-1);
	}
	else printf("serial com open success!\r\n");
    switch(baudrate_value)
    {
        case 2400:
            baud_val = B2400;
            break;
        case 4800:
            baud_val = B4800;
            break;
        case 9600:
            baud_val = B9600;
            break;
        case 38400:
            baud_val = B38400;
            break;
        case 57600:
            baud_val = B57600;
            break;
        case 115200:
            baud_val = B115200;
            break;
        default:
            baud_val = B9600;
            break;
    }


	tcgetattr(serial_port_fd, &oldtio);
    newtio.c_oflag = 0;
    newtio.c_iflag = 0;
    newtio.c_cflag &= ~CSIZE;
    newtio.c_cflag |= CS8;
    newtio.c_cflag |= (CLOCAL|CREAD);
    //newtio.c_cflag |= __BAUDRATE__;
    newtio.c_cflag &= ~CRTSCTS;
    newtio.c_cflag &= ~PARENB;
    newtio.c_iflag &= ~INPCK; 
    newtio.c_oflag &= ~OPOST; 
    //newtio.c_cflag |= PARENB;
    //newtio.c_cflag &= ~PARODD;
    newtio.c_cflag &= ~CSTOPB;
    newtio.c_lflag &= ~(ICANON|ECHO|ECHOE|ISIG);
    newtio.c_iflag &= ~(IXON|IXOFF|IXANY);

    newtio.c_iflag &= ~(INLCR|ICRNL|IGNCR);
    newtio.c_oflag &= ~(ONLCR|OCRNL);
    
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;
    newtio.c_cc[VINTR] = 0;
    newtio.c_cc[VERASE] = 0;
    newtio.c_cc[VKILL] = 0;
    newtio.c_cc[VEOF] = 0;
    newtio.c_cc[VSWTC] = 0;
    newtio.c_cc[VSTART] = 0;
    newtio.c_cc[VSTOP] = 0;
    newtio.c_cc[VSUSP] = 0;
    newtio.c_cc[VEOL] = 0;
    newtio.c_cc[VREPRINT] = 0;
    newtio.c_cc[VDISCARD] = 0;
    newtio.c_cc[VWERASE] = 0;
    newtio.c_cc[VLNEXT] = 0;
    newtio.c_cc[VEOL2] = 0;

    cfsetispeed(&newtio,baud_val);
    cfsetospeed(&newtio,baud_val);

    tcflush(serial_port_fd, TCIFLUSH);
    tcsetattr(serial_port_fd, TCSANOW, &newtio);

    printf("serial com setting OK!\r\n");
    return serial_port_fd;
}

int Init_Socket(char *IP, struct sockaddr_in *udp_sockaddr, int netport)
{
	int sock_fd;
	//struct sockaddr_in udp_arm;

	if((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket creat error!\r\n");
		return(-1);
	}
	else printf("socket creat success!\r\n");
	//bzero(&udp_arm, sizeof(udp_arm));
	udp_sockaddr->sin_family = AF_INET;
	udp_sockaddr->sin_port = htons(netport);
    //udp_sockaddr->sin_port = htons(__NET_PORT__);
	inet_pton(AF_INET, IP, &udp_sockaddr->sin_addr);
	//udp_arm.sin_addr = inet_addr(192.168.1.10);
	return sock_fd;
}

/*
void choose_serial_com(unsigned char *com_number, unsigned char *device_id)
{   
    unsigned char tty_id;

    memcpy(com_number, "/dev/", 5);  
    tty_id = *(device_id);
  
    switch (tty_id)
    {
        case '1': 
            memcpy(com_number+5,"ttyAM0", 6);
            break;
        case '2':
            memcpy(com_number+5,"ttyAM1", 6);
            break;
        case '3':
            memcpy(com_number+5,"ttyAM2", 6);
            break;
        case '4':
            memcpy(com_number+5,"ttyS0", 5);
            break;
        case '5':
            memcpy(com_number+5,"ttyS1", 5);
            break;
        case '6':
            memcpy(com_number+5,"ttyS2", 5);
            break;
        case '7':
            memcpy(com_number+5,"ttyS3", 5); 
            break;
        default:
            printf("wrong serial com!!!\n");
            printf("open default serial:ttyS3\n");
            memcpy(com_number+5,"ttyS3", 5);
            break; 
    }

    printf("You select serial:%s\n",com_number);
}
*/
