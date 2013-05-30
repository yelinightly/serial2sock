#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>

const unsigned char send_buffer[5] = {0x68, 0x04, 0x00, 0x04, 0x08};
const unsigned char x_buffer[10] = {0x01, 0x02, 0x03, 0x04,0x05,0x06,0x07,0x08,0x09,0x0a};
int alarm_flag = 0;

extern pthread_mutex_t com_lock;
extern pthread_cond_t com_ready;

void *set_timer()
{
	struct itimerval itv,oldtv;
	itv.it_interval.tv_sec = 2;
	itv.it_interval.tv_usec = 0;
	itv.it_value.tv_sec = 2;
	itv.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &itv, &oldtv);
}

void signal_handler(int m)
{
	alarm_flag = 1;

}





void *thread_query_fun(void *fd_com)
{
	int written_bytes = 0;
	//int sync_flag = 0;
	int com_status;
	int x_fd;
	int i = 0;
	unsigned char recv_buffer[14];
	int recv_bytes = 0;

	x_fd = *(int *)fd_com;
	printf("thread_query_fun starting!\r\n");

//	signal(SIGALRM, signal_handler);
//	set_timer();

 	while(1)
	{

//		if(alarm_flag)
//		{
//		alarm_flag = 0;

		//pthread_mutex_lock(&com_lock);
		//ioctl(x_fd, TIOCMGET, &com_status);
		//com_status &= ~TIOCM_RTS;
		//ioctl(x_fd, TIOCMSET, &com_status);

		//ioctl(x_fd, TIOCMGET, &com_status);
		//com_status |= TIOCM_CTS;
		//ioctl(x_fd, TIOCMSET, &com_status);

		written_bytes = write(x_fd, send_buffer, 5);

		//sync_flag = fsync(x_fd);
		if(written_bytes < 0) {
			printf("___errno is %d___\n",errno);
		}
		else printf("written bytes is %d\n",written_bytes );
		//usleep(500);

		//ioctl(x_fd, TIOCMGET, &com_status);
		//com_status |= TIOCM_RTS;
		//ioctl(x_fd, TIOCMSET, &com_status);

		//ioctl(x_fd, TIOCMGET, &com_status);
		//com_status &= ~TIOCM_CTS;
		//ioctl(x_fd, TIOCMSET, &com_status);
		
		//ioctl(x_fd, TIOCMGET, &com_status);
		//com_status |= TIOCM_DSR;
		//ioctl(x_fd, TIOCMSET, &com_status);
		//usleep(100000);
		//pthread_cond_signal(&com_ready);
		//pthread_mutex_unlock(&com_lock);
		//sleep(3);
		//recv_bytes = treadn(x_fd, recv_buffer, 14, 5000);
		//recv_bytes = treadn(port_fd, recv_buffer, 14, 2000);
		//recv_bytes = read(x_fd, recv_buffer, 14);
		//if(recv_bytes > 0)
		//{
		//	for(i=0; i<14; i++)
		//	{
		//		printf("#0x%.2x",recv_buffer[i]);
		//	}
		//	printf("\n#recv_bytes = %d#\n",recv_bytes);
		//}
		//bzero(recv_buffer,7);
		select_sleep(1,0);
//		}
	}
}
