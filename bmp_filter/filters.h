#pragma once

#include "bmp.h"


void apply_grayscale(BMPImage* image);
void apply_blur(BMPImage* image);
void apply_invert(BMPImage* image);
void apply_threshold(BMPImage* image, float threshold);
void apply_blurry_blur(BMPImage* image);
void apply_kernel(BMPImage* image, int kernelType);
void apply_sobel(BMPImage* image);
void apply_sepia(BMPImage* image);