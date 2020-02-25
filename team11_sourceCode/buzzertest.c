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

#define DRIVER_NAME             "/dev/cnbuzzer"
#define MAX_BUZZER_NUMBER               36

static int buzzer_fd = 0;


void keyBuzzer(){
	
	int buzzerNum = 21;
	write(buzzer_fd, &buzzerNum, 4);
	usleep(100000);
	write(buzzer_fd, 0, 4);

}



void WarningBuzzer(){ //사고 났을시 높은 음을 통해 경고
	int buzzerNum;


	buzzerNum = 36;
	write(buzzer_fd, &buzzerNum, 4);
	usleep(300000);
	buzzerNum = 35;
	write(buzzer_fd, &buzzerNum, 4);
	usleep(300000);
	buzzerNum = 34;
	write(buzzer_fd, &buzzerNum, 4);
	usleep(300000);
	buzzerNum = 36;
	write(buzzer_fd, &buzzerNum, 4);
	usleep(300000);
	buzzerNum = 35;
	write(buzzer_fd, &buzzerNum, 4);
	usleep(300000);
	buzzerNum = 34;
	write(buzzer_fd, &buzzerNum, 4);
	usleep(300000);
	buzzerNum = 36;
	write(buzzer_fd, &buzzerNum, 4);
	usleep(300000);
	buzzerNum = 35;
	write(buzzer_fd, &buzzerNum, 4);
	usleep(300000);
	buzzerNum = 34;
	write(buzzer_fd, &buzzerNum, 4);
	usleep(300000);
	write(buzzer_fd, 0, 4);
}


int lockOpen_Buzzer() {//입력 받은 비밀번호가 동일하면 도래미파
	int buzzerNum;

	
	buzzerNum = 13;
	write(buzzer_fd, &buzzerNum, 4);
	sleep(1);
	buzzerNum = 15;
	write(buzzer_fd, &buzzerNum, 4);
	sleep(1);
	buzzerNum = 17;
	write(buzzer_fd, &buzzerNum, 4);
	sleep(1);
	buzzerNum = 18;
	write(buzzer_fd, &buzzerNum, 4);
	sleep(1);
	write(buzzer_fd, 0, 4);
}

int lockClose_Buzzer(){//입력 받은 비밀번호가 다르다면 경고음
	int buzzerNum;

	
	buzzerNum = 24;
	write(buzzer_fd, &buzzerNum, 4);
	sleep(1);
	buzzerNum = 24;
	write(buzzer_fd, &buzzerNum, 4);
	sleep(1);
	buzzerNum = 24;
	write(buzzer_fd, &buzzerNum, 4);
	sleep(1);
	buzzerNum = 24;
	write(buzzer_fd, &buzzerNum, 4);
	sleep(1);
	write(buzzer_fd, 0, 4);
}


int openBuzzer()
{
	// open  driver 
	buzzer_fd = open(DRIVER_NAME, O_RDWR);

	if (buzzer_fd < 0)
	{
		perror("driver (//dev//cnbuzzer) open error.\n");
		return 1;
	}

	return buzzer_fd;
}

