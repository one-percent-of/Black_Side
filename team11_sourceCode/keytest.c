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
#define DRIVER_NAME "/dev/cnkey"
/*    

  
  
   [1]    [2] [3]  [98]
   [4]    [5] [6]  [enter]
   [7]    [8] [9]  [99]
   [99] [0] [99] [99]
   [99][99][99][99]
    
   [0]~[9]: password
   [98] : start set password
   [enter] : save password
   [99] : not use
   */



static int key_fd = 0;

int openKeypad()
{
    // open driver
    key_fd = open(DRIVER_NAME, O_RDWR);

    if (key_fd < 0)
    {
    perror("driver open error.\n");
    return 1;
    }

    return key_fd;
}

int getKeyNum()
{
    int rdata;

TEST:
    read(key_fd, &rdata, 4);
    if(rdata == 0) goto TEST;
    if(rdata == 4) rdata = 98; //4 -> 98
    if(rdata == 5) rdata = 4;   // 5 -> 4
    if(rdata == 6) rdata = 5;   // 6 -> 5
    if(rdata == 7) rdata = 6;   // 7 -> 6
    if(rdata == 8) rdata = 97;   // 8 -> 97
    if(rdata == 9) rdata = 7;   // 9 -> 7
    if(rdata == 10) rdata = 8;   // 10 -> 8
    if(rdata == 11) rdata = 9;   // 11 -> 9
    if(rdata == 12) rdata = 99;   // 12 -> 99
    if(rdata == 13) rdata = 99;   // 13 -> 99
    if(rdata == 14) rdata = 0;
    if(15 ==rdata || rdata==16) rdata = 99;   // 15~20 -> 99, not use
    printf("button:%d\n", rdata);

    return rdata;
}
