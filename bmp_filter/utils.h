#pragma once

#include "bmp.h"
#include<stdint.h>


void insert_subfolder(const char* base_path, const char* subfolder, char* out_path, size_t size);
void error(char* err);