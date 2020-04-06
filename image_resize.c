// Example of using the Image library

#include "Image.h"
#include "Helper.h"

int main(int argc, char * argv[]) {

	int file_size = 0;
	FILE * file = NULL;
	const unsigned char * image_buffer_in;

//	/*
	if (argc != 4) {
		printf("Give only three arguements target_width target_height base64string respectively\n");
		printf("EX:./resize 500 500 /9j/AwD......\n");
		exit(1);
	}

	int target_width = atoi(argv[1]);
	int target_height = atoi(argv[2]);

	int base64Length = strlen(argv[3]);
	Image_Resize(argv[3], base64Length, target_width, target_height);
//	*/
	
	/*
	// Test the implementation using the images in test_images folders
	// Steps 
	//	* Load Image from Disk
	//	* Converting to base64string
	//	* Passing the string to Image_Resize() function

	//Open the image file in binary mode
	file = fopen("./test_images/png/png3.png", "rb");
	//file = fopen("./test_images/jpg/jpg1.jpg", "rb");

	if (!file) {
		fprintf(stderr, "Couldn't open the file. Check the path\n");
		exit(1);
	}

	//seek the file pointer to the end of file (image file)
	fseek(file, 0, SEEK_END);
	//take the current position of the pointer (how many bytes the pointer has passed)
	file_size = ftell(file);
	//seek the pointer to the beginning of the file
	fseek(file, 0, SEEK_SET);

	//allocate dynamic memory for the image buffer
	image_buffer_in = (const unsigned char *)malloc(file_size + 100);

	//read the file and store in the previously allocated memory
	fread(image_buffer_in, file_size, sizeof(unsigned char), file);
	//close the file pointer
	fclose(file);

	int target_width = 500;
	int target_height = 500;

	unsigned char * encodedBase64 = base64_encode(image_buffer_in, file_size);
	unsigned int encoded_length = 4 * ((file_size + 2) / 3);
	Image_Resize(encodedBase64, encoded_length,target_width,target_height);
	*/
	return 0;
}