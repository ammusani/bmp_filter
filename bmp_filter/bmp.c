#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>

BMPImage* read_bmp(const char* filename) {
	if (!filename) return NULL;

	FILE* fp = fopen(filename, "rb");	// opening the file
	if (!fp) return NULL;

	BMPImage* image = malloc(sizeof(BMPImage)); // allocating the size of BMPImage structure, 14 Bytes File Header + 40 Bytes Image header + pixel pointer size (depending on machine)
	if (!image) return NULL;

	fread(&image->bfHeader, sizeof(BFHeader), 1, fp); 
	fread(&image->biHeader, sizeof(BIHeader), 1, fp);

	// if starting chars aren't BM, exit out
	if (image->bfHeader.bfType != 0x4D42) {
		free(image);
		image = NULL;
		fclose(fp);
		return NULL;
	}

	// moving the file pointer to start of the image
	fseek(fp, image->bfHeader.bfOffBits, SEEK_SET);

	// size of image in bytes
	size_t size = calculate_size(image);
	if (!size) {
		free(image);
		image = NULL;
		fclose(fp);
		return NULL;
	}
	// allocating image on heap
	image->pixels = malloc(size);
	if (!image->pixels) {
		free(image);
		image = NULL;
		fclose(fp);
		return NULL;
	}
	// reading the image
	fread(image->pixels, 1, size, fp);
	fclose(fp);
	return image;
}

int write_bmp(const char* filename, BMPImage* image) {
	if (!image || !filename) return 1;
	
	FILE* fp = fopen(filename, "wb");
	if (!fp) return 1;

	// wrting back the headers
	fwrite(&image->bfHeader, sizeof(BFHeader), 1, fp);
	fwrite(&image->biHeader, sizeof(BIHeader), 1, fp);

	// moving image to location of pixels
	fseek(fp, image->bfHeader.bfOffBits, SEEK_SET);

	//writing down the pixels
	fwrite(image->pixels, 1, image->biHeader.biSizeImage, fp);

	fclose(fp);
	return 0;
}

void free_bmp(BMPImage* image) {
	if (image) {
		// check if pixels are there then free them
		if (image->pixels) {
			free(image->pixels);
			image->pixels = NULL;
		}

		free(image);
		image = NULL;
	}
}

uint32_t padding(BMPImage* image) {
	if (!image) return 0;
	
	size_t width = image->biHeader.biWidth;
	size_t bitsPerPixel = image->biHeader.biBitCount;
	uint32_t raw_row_size = (width * bitsPerPixel + 7) / 8; // added 7 so we don't lose significant data in division, (80 + 7) / 8 = 10, but it will help when (79 + 7) / 8 = 10
	uint32_t padded_row_size = (raw_row_size + 3) & ~3; // adding 3 and clearing last 2 bits, i.e. removing spill over 
	return padded_row_size - raw_row_size;
}

size_t calculate_size(BMPImage* image) {
	if (!image) return 0;
	
	size_t size = image->biHeader.biSizeImage;
	if (size == 0) {
		int32_t height = abs(image->biHeader.biHeight);
		int32_t width = image->biHeader.biWidth;
		uint16_t bitCount = image->biHeader.biBitCount;

		if (bitCount != 1 && bitCount != 2 && bitCount != 4 && bitCount != 8 && bitCount != 16 && bitCount != 24 && bitCount != 32) return 0;

		size_t rowSize = ((width * bitCount + 31) / 32) * 4;
		size =  rowSize * height;

		
		image->biHeader.biSizeImage = size;
	}
	return size;
}