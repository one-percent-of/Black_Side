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

#define DRIVER_NAME		"/dev/cncled"
#define INDEX_LED		0
#define INDEX_RED_LED		1
#define INDEX_GREEN_LED		2
#define INDEX_BLUE_LED		3
#define INDEX_MAX		4

static int cled_fd;

int opencled() {

	cled_fd = open(DRIVER_NAME, O_RDWR);
	if (cled_fd < 0)
	{
		perror("driver  open error.\n");
		return 1;
	}
	return cled_fd;
}
int cledtest(int de,int red,int green,int blue) //메인에서 제어
{
	unsigned short colorArray[INDEX_MAX];

	colorArray[INDEX_LED] =de;
	if (colorArray[INDEX_LED] > 3 )
	{
		
		perror("led index  over range.\n");
		return 1;
	}
	colorArray[INDEX_RED_LED] =red;
	colorArray[INDEX_GREEN_LED] =green;
	colorArray[INDEX_BLUE_LED] =blue;
	
	write(cled_fd,&colorArray,6);
	return cled_fd;
}
