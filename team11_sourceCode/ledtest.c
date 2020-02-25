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
#include <time.h>

#define LED_ON	1
#define LED_OFF	0
#define MAX_LED_NO		8
#define LED_DRIVER_NAME		"/dev/cnled"

static int led_fd = 0;

int ledDisplay(int vib_val)
{
    
    int wdata, rdate, temp;
    int cnt = 0;
    int rand_num;
    // 진동강도에 따른 led 출력
    if (vib_val == 0) wdata = 0x00;
    if (10< vib_val && vib_val < 15) wdata = 0x01; //임계치 20이하이지만 확인 해야할경우 1개의 led 출력
    if (15< vib_val && vib_val < 20) wdata = 0x03;
    if (20< vib_val && vib_val < 25) wdata = 0x07;
    if (25< vib_val && vib_val < 30) wdata = 0x0F;
    if (30< vib_val && vib_val < 40) wdata = 0x1F;
    if (40< vib_val && vib_val < 45) wdata = 0x3F;
    if (45< vib_val && vib_val < 60) wdata = 0x7F;
    if (60< vib_val) wdata = 0xFF;
    
    
    write(led_fd, &wdata, 4);
}

/* busled드라이브 open */
int openLed()
{
    int wdata = 0x00;
    led_fd = open(LED_DRIVER_NAME, O_RDWR);
    write(led_fd,&wdata,4);
    if (led_fd < 0)
    {
        perror("driver (//dev//cnled) open error.\n");
        return 1;
    }
    
    return led_fd;
}


