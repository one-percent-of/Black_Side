#ifndef __BITMAP_H__
#define __BITMAP_H_


// BMP File Structure (windows version 3)
// File Header
#pragma pack(1)
typedef struct {
    unsigned char   bfType;         // 2 byte
    unsigned int    bfSize;         // 4 byte
    unsigned short  bfReserved1;    // 2 byte
    unsigned short  bfReserved2;    // 2 byte
    unsigned int    bfOffBits;      // 4 byte
} BITMAPFILEHEADER1;
#pragma pack()
// Image Header
typedef struct {
    unsigned int    biSize;             // 4 byte
    unsigned int    biWidth;            // 4 byte
    unsigned int    biHeight;           // 4 byte
    unsigned short  biPlanes;           // 2 byte
    unsigned short  biBitCount;         // 2 byte
    unsigned int    biCompression;      // 4 byte
    unsigned int    biSizeImage;        // 4 byte
    unsigned int    biXPelsPerMeter;    // 4 byte
    unsigned int    biYPelsPerMeter;    // 4 byte
    unsigned int    biClrUsed;          // 4 byte
    unsigned int    biClrImportant;     // 4 byte
} BITMAPINFOHEADER1;


typedef struct {
    //unsigned char   bfType;         // 2 byte
    unsigned int    bfSize;         // 4 byte
    unsigned short  bfReserved1;    // 2 byte
    unsigned short  bfReserved2;    // 2 byte
    unsigned int    bfOffBits;      // 4 byte
} BITMAPFILEHEADER;

// Image Header
typedef struct {
    unsigned int    biSize;             // 4 byte
    unsigned int    biWidth;            // 4 byte
    unsigned int    biHeight;           // 4 byte
    unsigned short  biPlanes;           // 2 byte
    unsigned short  biBitCount;         // 2 byte
    unsigned int    biCompression;      // 4 byte
    unsigned int    biSizeImage;        // 4 byte
    unsigned int    biXPelsPerMeter;    // 4 byte
    unsigned int    biYPelsPerMeter;    // 4 byte
    unsigned int    biClrUsed;          // 4 byte
    unsigned int    biClrImportant;     // 4 byte
} BITMAPINFOHEADER;

// Color Table
typedef struct {
// windows version 3
    unsigned char   rgbBlue;        // 1 byte
    unsigned char   rgbGreen;       // 1 byte
    unsigned char   rgbRed;         // 1 byte
    unsigned char   rgbReserved;    // 1 byte
} RGBQUAD;

int openBMP();
void bitmaptest(char* filename, unsigned char* pfbmap, int x_1, int y_1);
void read_bmp(char *filename, char **pDib, char **data, int *cols, int *rows);
void close_bmp(char **pDib);

#endif


