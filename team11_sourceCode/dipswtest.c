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

#define DRIVER_NAME		"/dev/cndipsw"

static int dip_fd = 0;

int getDipState() //딥스위치의 상황을 받아옴
{
	int retvalue;
	read(dip_fd, &retvalue, 4);
	retvalue &= 0xF;

	return retvalue;
}

int openDipsw() 
{
	// open  driver 
	dip_fd = open(DRIVER_NAME, O_RDWR); 
	if (dip_fd < 0)
	{
		perror("driver  open error.\n");
		return 1;
	}

	return dip_fd;
}
