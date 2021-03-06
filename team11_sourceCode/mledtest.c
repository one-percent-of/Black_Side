#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#define DRIVER_NAME		"/dev/cnmled"
#define ONE_LINE_TIME_U 1000
#define MAX_COLUMN_NUM 5


static int mled_fd = 0;

int mLedkbhit(void)
{
    struct timeval tv;
    fd_set rdfs;
    
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    
    
    FD_ZERO(&rdfs);
    FD_SET(STDIN_FILENO, &rdfs);
    
    select(STDIN_FILENO + 1, &rdfs, NULL, NULL, &tv);
    
    return FD_ISSET(STDIN_FILENO, &rdfs);
}


const unsigned short NumData[2][MAX_COLUMN_NUM] =
{
    {0xfe00, 0xfd08, 0xfb14, 0xf722, 0xef41}, // left
    { 0xfe41, 0xfd22, 0xfb14, 0xf708, 0xef00 }, // right
    //{0xfe00, 0xfd00, 0xfb00, 0xf700, 0xef00} // NULL
};


int displayDotLed(int fd, int num, int timeS)
{   
	unsigned short wdata[2];
	int cSelCounter, loopCounter;
	int totalCount;

	totalCount = timeS*(1000000 / ONE_LINE_TIME_U);  //timeS = 5 따라서 5초동안 띄운다 
	printf("totalcounter: %d\n", totalCount);    

	cSelCounter = 0;
	loopCounter = 0;

	//wdata[0]은 왼쪽 Dotmatrix
	//wdata[1]은 오른쪽 Dotmatrix
	while (1) {
		if (num == 1) {  
			wdata[0] = NumData[0][cSelCounter];  //<<
			//wdata[1] = NumData[1][cSelCounter];    
		}
		else if (num ==0) {           
			//wdata[0] = NumData[2][cSelCounter];
			wdata[1] = NumData[1][cSelCounter];//>>
		} 
		else{
			wdata[0] = 0;
			wdata[1] = 0;
		}

		write(fd, (unsigned char*)wdata, 4);

		cSelCounter++;
		if (cSelCounter > (MAX_COLUMN_NUM - 1))
			cSelCounter = 0;
		usleep(ONE_LINE_TIME_U);

		loopCounter++; 
		if (loopCounter > totalCount) break;
	}
	
	wdata[0] = 0;
	wdata[1] = 0;
	write(fd, (unsigned char*)wdata, 4);
	return 0;
}

int mledDisplay(int left,int right) 
{
	int durationTime = 5, Num;

	if (left ==1 ) Num = 1;
	else if (right ==1) Num = 0;
	else {
		Num =2;
		durationTime = 1;
	}
	displayDotLed(mled_fd, Num, durationTime);
}

int openMled()
{
    mled_fd = open(DRIVER_NAME, O_RDWR);
    
    if (mled_fd < 0) {
        perror("driver open error.\n");
        return -1;
    }
    
    return mled_fd;
}

