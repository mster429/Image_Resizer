#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define PNG 1
#define JPG 2

enum allocation_type {
	NO_ALLOCATION, SELF_ALLOCATED, STB_ALLOCATED
};

//Image Structure Used to hold image data and decompressed pixel data
typedef struct {
	int width;
	int height;
	int channels;
	size_t size;
	unsigned char *data;
	enum allocation_type allocation_;
} Image;

//Structure to hold compressed JPG image data (For PNG not needed)
typedef struct {
	unsigned long size;
	unsigned char *data;
}JPEGIMAGE;

void Image_load(Image *img, const unsigned char * buffer, int len); //Load image byte array from memory to Image structure
void Image_load_from_disk(Image *img, const char *fname);  //Load image byte array from disk to Image structure
void Image_Resize(unsigned char * base64String, unsigned int stringLen, int target_width, int target_height); //Image resize function
JPEGIMAGE compressJPEG(Image bitmap, int quality);			//compress JPG Image structure to byte array in memory
void Image_create(Image *img, int width, int height, int channels, bool zeroed); //create Image structure 
void Image_save(const Image *img, const char *fname, int type);	//save data in Image structure to a image (.png or .jpg) file
void Image_free(Image *img);	//free memory allocated to Image structure
