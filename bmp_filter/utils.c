#include "bmp.h"

#include<string.h>
#include<stdio.h>
#include<stdint.h>

// create files in subfolders
void insert_subfolder(const char* base_path, const char* subfolder, char* out_path, size_t size) {
	const char* last_slash = strrchr(base_path, '\\');
	if (!last_slash) {
		// No directory path found
		snprintf(out_path, size, "%s%s", subfolder, base_path);
		return;
	}

	size_t dir_len = last_slash - base_path + 1; // include slash
	snprintf(out_path, size, "%.*s%s%s", (int)dir_len, base_path, subfolder, last_slash + 1);
}


void error(char* err) {
	printf("%s\n", err);
	exit(1);
}