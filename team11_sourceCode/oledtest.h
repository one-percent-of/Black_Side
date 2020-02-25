#ifndef __OLEDTEST_H__
#define __OLEDTEST_H__

//int oLedreset(void);
//int writecmd(int size , unsigned short* cmdArr);
//int writeData(int size , unsigned char* dataArr);
//int readData(int size , unsigned short* dataArr);
//int setAddressDefalut(void);

//int setCmdLock(int bLock);
//int oLedimageLoading(char* fileName);
//int oLedInit(void);
int openOledDev();
int oledtest(int Mode);

/*#define MODE_WRITE		0
#define MODE_READ		1
#define MODE_CMD		2
#define OLED_MODE_RESET		3
#define OLED_MODE_IMAGE		4
#define OLED_MODE_INIT		5
*/
#endif
