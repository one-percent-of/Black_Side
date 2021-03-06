#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h> // for O_RDWR
#include <sys/ioctl.h> // for ioctl
#include <sys/mman.h>
#include <linux/fb.h> 
#include <signal.h>
#include <linux/input.h>
#include <string.h>

#include "bitmap.h"

#define FBDEV_FILE "/dev/fb0"
#define BIT_VALUE_24BIT 24
#define MAX_BUFF 200
#define MAX_TOUCH_X 0x740
#define MAX_TOUCH_Y 0x540

int		screen_width = 1280;
int		screen_height = 800;

#define MAX_TOUCH_X	0x740
#define MAX_TOUCH_Y	0x540


static int touch_fd=0;
static int past_touchX = 0;
static int past_touchY = 0;
static unsigned char* pfbmap;
static int mem_size = 0;


int openBMP(){ 
	
	int bit_fd;
	int screen_width = 1280;
	int screen_height = 800;
	int coor_x, coor_y;
	unsigned long *ptr;
	struct fb_var_screeninfo fbvar;
	struct fb_fix_screeninfo fbfix;

	bit_fd = open(FBDEV_FILE, O_RDWR);

	if (bit_fd<0)
	{
		printf("%s:open error\n", FBDEV_FILE);
		exit(1);
	}

	if (ioctl(bit_fd, FBIOGET_VSCREENINFO, &fbvar))
	{
		printf("%s: itctl erro - FBIOGET_VSCREENINFO \n", FBDEV_FILE);
		exit(1);
	}

	if (ioctl(bit_fd, FBIOGET_FSCREENINFO, &fbfix))
	{
		printf("%s: ioctl error - FBIOGET_FSCREENINFO \n", FBDEV_FILE);
		exit(1);
	}

	mem_size = screen_width * screen_height * 4;
	pfbmap = (unsigned char *)mmap(0, mem_size,
		PROT_READ | PROT_WRITE, MAP_SHARED, bit_fd, 0);

	for (coor_y = 0; coor_y < screen_height; coor_y++)
	{
		ptr = (unsigned long*)pfbmap + screen_width * coor_y;
		for (coor_x = 0; coor_x < screen_width; coor_x++)
		{
			*ptr++ = 0xFFFFFF;
		}
	}
	
	return bit_fd;

}

void read_bmp(char *filename, char **pDib, char **data, int *cols, int *rows) // bmp파일을 읽음 
{
	BITMAPFILEHEADER bmpHeader;
	BITMAPINFOHEADER *bmpInfoHeader;
	unsigned int size;
	unsigned char magicNum[2];
	int nread;
	FILE *fp;
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		printf("ERROR\n");
		return;
	}
	
	magicNum[0] = fgetc(fp);
	magicNum[1] = fgetc(fp);
	if (magicNum[0] != 'B' && magicNum[1] != 'M') {
		printf("It's not a bmp file!\n");
		fclose(fp);
		return;
	}
	nread = fread(&bmpHeader.bfSize, 1, sizeof(BITMAPFILEHEADER), fp);
	size = bmpHeader.bfSize - sizeof(BITMAPFILEHEADER);
	*pDib = (unsigned char *)malloc(size); // DIB Header(Image Header)
	fread(*pDib, 1, size, fp);
	bmpInfoHeader = (BITMAPINFOHEADER *)*pDib;
	
	if (BIT_VALUE_24BIT != (bmpInfoHeader->biBitCount)) //24bit bmp파일인지 확인
	{
		printf("It supports only 24bit bmp!\n");
		fclose(fp);
		return;
	}
	*cols = bmpInfoHeader->biWidth;
	*rows = bmpInfoHeader->biHeight;
	*data = (char *)(*pDib + bmpHeader.bfOffBits - sizeof(bmpHeader) - 2);
	fclose(fp);   //// get data from fp.bmp file ( width, height...)
}

void bitmaptest(char* filename, unsigned char* pfbmap, int x, int y)
{
	int i, j, k, t;
	int bits_per_pixel;
	int line_length;
	int coor_x, coor_y;
	int cols = 0, rows = 0;

	char *pData, *data;
	char r, g, b;
	unsigned long bmpdata[1280 * 800];
	unsigned long pixel;
	unsigned long *ptr;


	int screen_width = 1280;
	int screen_height = 800;


	read_bmp(filename, &pData, &data, &cols, &rows);
	for (j = 0; j<rows; j++)
	{
		k = j*cols * 3;
		t = (rows - 1 - j) * cols;

		for (i = 0; i<cols; i++)
		{
			b = *(data + (k + i * 3));
			g = *(data + (k + i * 3 + 1));
			r = *(data + (k + i * 3 + 2));

			pixel = ((r << 16) | (g << 8) | b);
			bmpdata[t + i] = pixel;
		}
	}

	close_bmp(&pData);
	if ((unsigned)pfbmap == (unsigned)-1) {
		perror("fbdev mmap\n");
		exit(1);
	}

	// data copy
	for (coor_y = 0; coor_y<rows; coor_y++) {
		ptr = (unsigned long*)pfbmap + x + (screen_width*(coor_y + y));
		for (coor_x = 0; coor_x<cols; coor_x++) {
			*ptr++ = bmpdata[coor_x + coor_y*cols];
		}
	}
}


void close_bmp(char **pDib)
{
	free(*pDib);  
}
