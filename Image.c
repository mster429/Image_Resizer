#include <stdio.h>
#include <string.h>

#include "Image.h"
#include "Helper.h"
#include <math.h>
#include "include/jpeglib.h" 

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image/stb_image_resize.h"


void Image_Resize(unsigned char * base64String, unsigned int stringLen, int target_width, int target_height) {
	//Image structures to hold input and resized image data
	Image input_image = (Image) { 0,0,0};
	Image resized_image = (Image) { 0, 0, 0};

	unsigned char * image_buffer_out = NULL;
	unsigned char * output_base64 = NULL;

	//Decoding base64 string data to byte array (compressed)
	unsigned char * image_buffer_in = base64_decode(base64String, stringLen);
	//byte array size
	int buffer_size = stringLen / 4 * 3;
	//load image data (pixel array, height ,width ..) to Image structure
	Image_load(&input_image, image_buffer_in, buffer_size);
	//stride is pixels per a row in image
	int stride_bytes_input = input_image.width * input_image.channels;
	int stride_bytes_output = resized_image.width * resized_image.channels;
	//Create a Image structure for the resized image
	Image_create(&resized_image, target_width, target_height, input_image.channels, true);
	//Resizing image to given dimensions
	stbir_resize_uint8(input_image.data, input_image.width, input_image.height, stride_bytes_input,
		               resized_image.data, target_width, target_height, stride_bytes_output,input_image.channels);
	
	/*
		* Procedure to identify image type from base64 string 
		* For JPG the compressed byte array contains "JFIF" in data[6-9]
		* For JPG the compressed byte array contains "PNG" in data[1-3]
	*/
	char jpeg[5] = { '0','0','0','0','\0' };
	char png[4] = { '0','0','0','\0' };
	
	//Copying relevant chars to above arrays
	memcpy(&jpeg, &image_buffer_in[6], 4);
	memcpy(&png, &image_buffer_in[1], 3);
	unsigned int encoded_length = 0;

	//detect the image type
	if (strcmp(jpeg, "JFIF") == 0) {
		JPEGIMAGE image_out = compressJPEG(resized_image, 100);	//create JPEGIMAGE struct to hold compressed data in memory  
		output_base64 = base64_encode(image_out.data, image_out.size);	//encode compressed data back to base64 string
		writeByteArrayToFile(image_out.data, image_out.size, JPG);	//writing output image file to disk
		Image_save(&input_image,"./input.jpg",JPG);	//writing input image file to disk
		encoded_length = 4 * ((image_out.size + 2) / 3);
		for (int i = 0; i < encoded_length; ++i) {
			printf("%c", output_base64[i]);
		}
	}
	else if (strcmp(png, "PNG") == 0) {
		int png_file_length = 0;
		image_buffer_out = stbi_write_png_to_mem(resized_image.data, stride_bytes_output, resized_image.width,
			                                     resized_image.height, resized_image.channels, &png_file_length);	//compressed png to memory buffer
		output_base64 = base64_encode(image_buffer_out, png_file_length);	//encode compressed data back to base64 string
		encoded_length = 4 * ((png_file_length + 2) / 3);
		writeByteArrayToFile(image_buffer_out, png_file_length, PNG);	//writing output image file to disk
		Image_save(&input_image, "./input.png", PNG);	//writing input image file to disk

		for (int i = 0; i < encoded_length; ++i) {
			printf("%c", output_base64[i]);
		}
	}

	Image_free(&input_image);
	Image_free(&resized_image);
	//return output_base64;

}

void Image_load_from_disk(Image *img, const char *fname) {
	if ((img->data = stbi_load(fname, &img->width, &img->height, &img->channels, 0)) != NULL) {
		img->size = img->width * img->height * img->channels;
		img->allocation_ = STB_ALLOCATED;
	}
}


void Image_load(Image * img, const unsigned char * buffer, int len) {
	if ((img->data = stbi_load_from_memory(buffer, len, &img->width, &img->height, &img->channels, 0)) != NULL) {
		img->size = img->width * img->height * img->channels;
		img->allocation_ = STB_ALLOCATED;
	}
}


void Image_create(Image *img, int width, int height, int channels, bool zeroed) {
	size_t size = width * height * channels;
	if (zeroed) {
		img->data = calloc(size, 1);
	}
	else {
		img->data = malloc(size);
	}

	if (img->data != NULL) {
		img->width = width;
		img->height = height;
		img->size = size;
		img->channels = channels;
		img->allocation_ = SELF_ALLOCATED;
	}
}

void Image_save(const Image *img, const char *fname, int type) {
	// Check if the file name ends in one of the .jpg/.JPG/.jpeg/.JPEG or .png/.PNG
	if(type == PNG) stbi_write_png(fname, img->width, img->height, img->channels, img->data, img->width * img->channels);
	if(type == JPG) stbi_write_jpg(fname, img->width, img->height, img->channels, img->data, 100);
}

void Image_free(Image *img) {
	if (img->allocation_ != NO_ALLOCATION && img->data != NULL) {
		if (img->allocation_ == STB_ALLOCATED) {
			stbi_image_free(img->data);
		}
		else {
			free(img->data);
		}
		img->data = NULL;
		img->width = 0;
		img->height = 0;
		img->size = 0;
		img->allocation_ = NO_ALLOCATION;
	}
}

//compressing raw pixel and image header data to jpg byte array in memory

JPEGIMAGE compressJPEG(Image bitmap,int quality) {
	unsigned char * image_buffer_out = NULL;
	unsigned char * bitmapBuffer = bitmap.data;
	unsigned long out_buffer_size = 0;
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
	int row_stride;		/* physical row width in image buffer */

	jpeg_mem_dest(&cinfo, &image_buffer_out, &out_buffer_size);

	/* Step 3: set parameters for compression */

	/* First we supply a description of the input image.
	 * Four fields of the cinfo struct must be filled in:
	 */
	cinfo.image_width = bitmap.width; 	/* image width and height, in pixels */
	cinfo.image_height = bitmap.height;
	cinfo.input_components = bitmap.channels;		/* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, 100, TRUE);
	jpeg_start_compress(&cinfo, TRUE);

	row_stride = bitmap.width * 3;	/* JSAMPLEs per row in image_buffer */
	while (cinfo.next_scanline < cinfo.image_height) {
		/* jpeg_write_scanlines expects an array of pointers to scanlines.
		 * Here the array is only one element long, but you could pass
		 * more than one scanline at a time if that's more convenient.
		 */
		row_pointer[0] = &bitmapBuffer[cinfo.next_scanline * row_stride];
		(void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}
	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	JPEGIMAGE resized_jpeg;
	resized_jpeg.data = image_buffer_out;
	resized_jpeg.size = out_buffer_size;

	return resized_jpeg;
}


