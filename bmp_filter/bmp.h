#pragma once

#include<stdint.h>

// saving old packing size, and setting current to 1, i.e. there will be no padding
#pragma pack(push, 1)		
typedef struct {
	uint16_t bfType;		// charatercs BM
	uint32_t bfSize;		// size of file
	uint16_t bfReserved1;	// unused - set to 0
	uint16_t bfReserved2;	// unused - set to 0
	uint32_t bfOffBits;		// offset to start pixel data
} BFHeader;					// 14 bytes total bit file header

typedef struct {
	uint32_t biSize;		// header size, at least 40
	int32_t biWidth;		// image width in pixel
	int32_t biHeight;		// image height in pixel --> if biHeight > 0, 1st row of file is bottom row image, vice versa for negative
	uint16_t biPlanes;		// number of planes, always 1
	uint16_t biBitCount;	// bits per pixel, 8 or less bits use a color pallete, more use true color, where each byte represents one of red, blue, green. 16, 32 are color masked
	uint32_t biCompression; // 0-> uncompressed, 1 -> RLE-8, 2 -> RLE-4, 3 -> bitfields, not of much interest
	uint32_t biSizeImage;   // size of pixel data in bytes
	int32_t biXPelsPerMeter;	// x direction resolution preference
	int32_t biYPelsPerMeter;	// y direction resolution preference
	uint32_t biClrUsed;			// usually 0, but it specifies colors used
	uint32_t biClrImportant;	// number of important colors
} BIHeader;					//bitmap info header - usually 40 bytes
#pragma pack(pop)

typedef struct {
	BFHeader bfHeader;
	BIHeader biHeader;
	unsigned char* pixels;
} BMPImage;

BMPImage* read_bmp(const char* filename);
int write_bmp(const char* filename, BMPImage* image);
void free_bmp(BMPImage* image);
uint32_t padding(BMPImage* image);
size_t calculate_size(BMPImage* image);