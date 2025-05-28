#include "bmp.h"
#include "filters.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	if (argc < 4) error("Usage: %s <input_file> <output_file> <type>\n type:\n gray_scale = 1 \n invert_colors \n threshold = 3 \n blur = 4", argv[0]);

	char path[256];

	BMPImage* image = read_bmp(argv[1]);
	if (!image) {
		printf("Failed to load %s\n", argv[1]);
		exit(1);
	}

	switch (atoi(argv[3])) {
		case 1:
			apply_grayscale(image);
			insert_subfolder(argv[2], "gray\\", path, sizeof(path));
			break;
		case 2:
			apply_invert(image);
			insert_subfolder(argv[2], "invert\\", path, sizeof(path));
			break;
		case 3: 
			if (argc < 5) error("Usage: %s <input_file> <output_file> <type>\n type:\n gray_scale = 1 \n invert_colors ", argv[0]);
			char* endptr;
			float threshold = strtof(argv[4], &endptr);
			if (*endptr != '\0') error("invalid threshold");
			apply_threshold(image, threshold);
			insert_subfolder(argv[2], "threshold\\", path, sizeof(path));
			break;
		case 4:
			apply_blur(image);
			insert_subfolder(argv[2], "blur\\", path, sizeof(path));
			break;
		case 5:
			apply_blurry_blur(image);
			insert_subfolder(argv[2], "blurry_blur\\", path, sizeof(path));
			break;
		case 6:
			apply_kernel(image, 1);
			insert_subfolder(argv[2], "sharpen\\", path, sizeof(path));
			break;
		case 7:
			apply_kernel(image, 2);
			insert_subfolder(argv[2], "emboss\\", path, sizeof(path));
			break;
		case 8:
			apply_grayscale(image);
			apply_sobel(image);
			insert_subfolder(argv[2], "sobel\\", path, sizeof(path));
			break;
		case 9:
			apply_sepia(image);
			insert_subfolder(argv[2], "sepia\\", path, sizeof(path));
			break;
		default:
			free_bmp(image);
			error("unknown type\n");
			
	}
	
	if (write_bmp(path, image)) {
		error("Failed to write to %s\n", argv[2]);
	}
	
	free_bmp(image);
	return 0;
}