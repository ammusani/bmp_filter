#include "bmp.h"
#include "utils.h"
#include<stdint.h>
#include<string.h>
#include <stdlib.h>
#include <math.h>

void apply_grayscale(BMPImage *image) {
	// assumption either the image is 32 bit or 24 bit and padded
	int32_t width = image->biHeader.biWidth;
	int32_t height = image->biHeader.biHeight;
	if (height < 0) height = -height;
	int32_t bytesPP = image->biHeader.biBitCount / 8;

	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t gray;
	uint32_t rowPadding = padding(image);

	uint8_t* pixelPtr = image->pixels;

	for (int32_t i = 0; i < height; i++) {
		for (int32_t j = 0; j < width; j++) {
			b = *pixelPtr;
			g = *(pixelPtr + 1);
			r = *(pixelPtr + 2);

			gray = 0.299 * (float)r + 0.587 * (float)g + 0.114 * (float)b;

			*pixelPtr = gray;
			*(pixelPtr + 1) = gray;
			*(pixelPtr + 2) = gray;
			pixelPtr += bytesPP;
		}
		pixelPtr += rowPadding;
	}

}

void apply_blur(BMPImage* image) {
	// assumption either the image is 32 bit or 24 bit and padded
	int32_t width = image->biHeader.biWidth;
	int32_t height = image->biHeader.biHeight;
	int isBottomUp = height > 0;
	if (height < 0) height = -height;
	uint32_t bytesPP = image->biHeader.biBitCount / 8;

	uint32_t rowPadding = padding(image);
	uint32_t paddedRowSize = width * bytesPP + rowPadding;

 	uint8_t* pixelPtr = image->pixels;
	uint8_t* outPtr = malloc(image->biHeader.biSizeImage);
	if (!outPtr) error("memory allocation failed");

	for (int32_t i = 1; i < height - 1; i++) {
		for (int32_t j = 1; j < width - 1; j++) {
			
			float b = 0.0;
			float g = 0.0;
			float r = 0.0;
			for (int k = -1; k < 2; k++) {
				for (int l = -1; l < 2; l++) {
					int32_t ni = i + k;
					int32_t nj = j + l;

					uint8_t* pixel = pixelPtr + ni * paddedRowSize + nj * bytesPP;

					b += *pixel;
					g += *(pixel + 1);
					r += *(pixel + 2);
				}
			}

			uint8_t* outPixel = outPtr + i * paddedRowSize + j * bytesPP;
			if (!outPixel) error("empty pointer encountered");
			*outPixel = (uint8_t)(b / 9.0);
			*(outPixel + 1) = (uint8_t)(g / 9.0);
			*(outPixel + 2) = (uint8_t)(r / 9.0);
			
		}
		
	}
	memcpy(pixelPtr, outPtr, image->biHeader.biSizeImage);

}

void apply_blurry_blur(BMPImage* image) {
	// assumption either the image is 32 bit or 24 bit and padded
	int32_t width = image->biHeader.biWidth;
	int32_t height = image->biHeader.biHeight;
	int isBottomUp = height > 0;
	if (height < 0) height = -height;
	uint32_t bytesPP = image->biHeader.biBitCount / 8;

	uint32_t rowPadding = padding(image);
	uint32_t paddedRowSize = width * bytesPP + rowPadding;

	uint8_t* pixelPtr = image->pixels;
	uint8_t* outPtr = malloc(image->biHeader.biSizeImage);
	if (!outPtr) error("memory allocation failed");

	for (int32_t i = 3; i < height - 3; i++) {
		for (int32_t j = 3; j < width - 3; j++) {

			float b = 0.0;
			float g = 0.0;
			float r = 0.0;
			for (int k = -3; k < 4; k++) {
				for (int l = -3; l < 4; l++) {
					int32_t ni = i + k;
					int32_t nj = j + l;

					uint8_t* pixel = pixelPtr + ni * paddedRowSize + nj * bytesPP;

					b += *pixel;
					g += *(pixel + 1);
					r += *(pixel + 2);
				}
			}

			uint8_t* outPixel = outPtr + i * paddedRowSize + j * bytesPP;
			if (!outPixel) error("empty pointer encountered");
			*outPixel = (uint8_t)(b / 49.0);
			*(outPixel + 1) = (uint8_t)(g / 49.0);
			*(outPixel + 2) = (uint8_t)(r / 49.0);

		}

	}
	memcpy(pixelPtr, outPtr, image->biHeader.biSizeImage);

}

void apply_invert(BMPImage* image) {
	// assumption either the image is 32 bit or 24 bit and padded
	int32_t width = image->biHeader.biWidth;
	int32_t height = image->biHeader.biHeight;
	if (height < 0) height = -height;
	int32_t bytesPP = image->biHeader.biBitCount / 8;
	uint32_t rowPadding = padding(image);

	uint8_t* pixelPtr = image->pixels;

	for (int32_t i = 0; i < height; i++) {
		for (int32_t j = 0; j < width; j++) {
			*pixelPtr = 255 - *(pixelPtr);
			*(pixelPtr + 1) = 255 - *(pixelPtr + 1);
			*(pixelPtr + 2) = 255 - *(pixelPtr + 2);
			pixelPtr += bytesPP;
		}
		pixelPtr += rowPadding;
	}

}

void apply_threshold(BMPImage* image, float threshold) {
	int32_t width = image->biHeader.biWidth;
	int32_t height = image->biHeader.biHeight;
	if (height < 0) height = -height;
	int32_t bytesPP = image->biHeader.biBitCount / 8;
	int32_t rowsize = (width * bytesPP + 3) & ~3;
	uint32_t rowPadding = padding(image);

	uint8_t* pixelPtr = image->pixels;

	for (int32_t i = 0; i < height; i++) {
		for (int32_t j = 0; j < width; j++) {
			float pixel = ((float)(*pixelPtr + *(pixelPtr + 1) + *(pixelPtr + 2))) / (255.0 * 3.0);
			if (pixel > threshold) {
				*pixelPtr = 255;
				*(pixelPtr + 1) = 255;
				*(pixelPtr + 2) = 255;
			}
			else {
				*pixelPtr = 0;
				*(pixelPtr + 1) = 0;
				*(pixelPtr + 2) = 0;
			}
			
			pixelPtr += bytesPP;
		}
		pixelPtr += rowPadding;
	}
}

void apply_kernel(BMPImage* image, int kernelType) {
	// assumption either the image is 32 bit or 24 bit and padded
	int32_t width = image->biHeader.biWidth;
	int32_t height = image->biHeader.biHeight;
	int isBottomUp = height > 0;
	if (height < 0) height = -height;
	uint32_t bytesPP = image->biHeader.biBitCount / 8;

	uint32_t rowPadding = padding(image);
	uint32_t paddedRowSize = width * bytesPP + rowPadding;

	uint8_t* pixelPtr = image->pixels;
	uint8_t* outPtr = malloc(image->biHeader.biSizeImage);
	if (!outPtr) error("memory allocation failed");

	int *kernel;

	switch (kernelType) {
		case 1: {
			static int sharpen[9] = { 0, -1, 0, -1, 5, -1, 0, -1, 0 };
			kernel = sharpen;
			break;
		}
		case 2: {
			static int emboss[9] = { -2, -1, 0, -1, 1, 1, 0, 1, 2 };
			kernel = emboss;
			break;
		}
		default: return;
	}

	for (int32_t i = 1; i < height - 1; i++) {
		for (int32_t j = 1; j < width - 1; j++) {

			int b = 0.0;
			int g = 0.0;
			int r = 0.0;
			for (int k = -1; k < 2; k++) {
				for (int l = -1; l < 2; l++) {
					int32_t ni = i + k;
					int32_t nj = j + l;

					uint8_t* pixel = pixelPtr + ni * paddedRowSize + nj * bytesPP;
					int kernelIndex = (k + 1) * 3 + (l + 1);
					b += (*pixel) * kernel[kernelIndex];
					g += (*(pixel + 1)) * kernel[kernelIndex];
					r += (*(pixel + 2)) * kernel[kernelIndex];
				}
			}

			uint8_t* outPixel = outPtr + i * paddedRowSize + j * bytesPP;
			if (!outPixel) error("empty pointer encountered");
			*outPixel = b > 255 ? 255 : (b < 0 ? 0 : b);
			*(outPixel + 1) = g > 255 ? 255 : (g < 0 ? 0 : g);
			*(outPixel + 2) = r > 255 ? 255 : (r < 0 ? 0 : r);

		}

	}
	memcpy(pixelPtr, outPtr, image->biHeader.biSizeImage);

}

void apply_sobel(BMPImage* image) {
	// assumption either the image is 32 bit or 24 bit and padded
	int32_t width = image->biHeader.biWidth;
	int32_t height = image->biHeader.biHeight;
	int isBottomUp = height > 0;
	if (height < 0) height = -height;
	uint32_t bytesPP = image->biHeader.biBitCount / 8;

	uint32_t rowPadding = padding(image);
	uint32_t paddedRowSize = width * bytesPP + rowPadding;

	uint8_t* pixelPtr = image->pixels;
	uint8_t* outPtr = malloc(image->biHeader.biSizeImage);
	if (!outPtr) error("memory allocation failed");

	static int xKernel[9] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };
	static int yKernel[9] = { -1, -2, -1, 0, 0, 0, -1, 2, 1 };

	for (int32_t i = 1; i < height - 1; i++) {
		for (int32_t j = 1; j < width - 1; j++) {

			int gX = 0;
			int gY = 0;

			for (int k = -1; k < 2; k++) {
				for (int l = -1; l < 2; l++) {
					int32_t ni = i + k;
					int32_t nj = j + l;

					uint8_t* pixel = pixelPtr + ni * paddedRowSize + nj * bytesPP;
					int kernelIndex = (k + 1) * 3 + (l + 1);
					gX += (*pixel) * xKernel[kernelIndex];
					gY += (*pixel) * yKernel[kernelIndex];
					
				}
			}

			uint8_t* outPixel = outPtr + i * paddedRowSize + j * bytesPP;
			if (!outPixel) error("empty pointer encountered");
			int outG = (int)sqrt(gX * gX + gY * gY);
			outG = outG > 255 ? 255 : outG;
			*outPixel = outG;
			*(outPixel + 1) = outG;
			*(outPixel + 2) = outG;

		}

	}
	memcpy(pixelPtr, outPtr, image->biHeader.biSizeImage);
}

void apply_sepia(BMPImage* image) {
	// assumption either the image is 32 bit or 24 bit and padded
	int32_t width = image->biHeader.biWidth;
	int32_t height = image->biHeader.biHeight;
	if (height < 0) height = -height;
	int32_t bytesPP = image->biHeader.biBitCount / 8;

	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t gray;
	uint32_t rowPadding = padding(image);

	uint8_t* pixelPtr = image->pixels;

	for (int32_t i = 0; i < height; i++) {
		for (int32_t j = 0; j < width; j++) {
			b = *pixelPtr;
			g = *(pixelPtr + 1);
			r = *(pixelPtr + 2);

			uint8_t newB = 0.393 * (float)r + 0.769 * (float)g + 0.189 * (float)b;
			uint8_t newG = 0.349 * (float)r + 0.686 * (float)g + 0.168 * (float)b;
			uint8_t newR = 0.272 * (float)r + 0.534 * (float)g + 0.131 * (float)b;

			*pixelPtr = newB;
			*(pixelPtr + 1) = newG;
			*(pixelPtr + 2) = newR;
			pixelPtr += bytesPP;
		}
		pixelPtr += rowPadding;
	}

}