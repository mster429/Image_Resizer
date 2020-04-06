#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "Helper.h"
#include "Image.h"

unsigned char * base64_encode(const unsigned char *data, size_t input_length);
void build_decoding_table();
unsigned char * base64_decode(const char * data, size_t input_length);
void base64_cleanup();

static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

static char *decoding_table = NULL;
static int mod_table[] = { 0, 2, 1 };
static char encoding_table[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
								 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
								 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
								 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
								 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
								 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
								 'w', 'x', 'y', 'z', '0', '1', '2', '3',
								 '4', '5', '6', '7', '8', '9', '+', '/' };


unsigned char * base64_encode(const unsigned char * data, size_t input_length) {
	int output_length = 4 * ((input_length + 2) / 3);
	unsigned char *encoded_data = malloc(output_length);
	if (encoded_data == NULL) return NULL;

	for (int i = 0, j = 0; i < input_length;) {

		uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
		uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
		uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

		uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

		encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
	}

	for (int i = 0; i < mod_table[input_length % 3]; i++)
		encoded_data[output_length - 1 - i] = '=';
	/*
	    for(char * curr=encoded_data; curr< &encoded_data[output_length];++curr){
	        printf("%c",*curr);
	    }
	*/
	/*
		for(int i=0; i< output_length;++i){
			printf("%c",encoded_data[i]);
		}
	*/

	return encoded_data;
}

unsigned char * base64_decode(const char * data, size_t input_length) {
	if (decoding_table == NULL) build_decoding_table();

	if (input_length % 4 != 0) return NULL;

	size_t output_length = input_length / 4 * 3;
	if (data[input_length - 1] == '=') (output_length)--;
	if (data[input_length - 2] == '=') (output_length)--;

	unsigned char * decoded_data = malloc(output_length);
	if (decoded_data == NULL) return NULL;

	for (int i = 0, j = 0; i < input_length;) {
		uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
		uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
		uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
		uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

		uint32_t triple = (sextet_a << 3 * 6)
			+ (sextet_b << 2 * 6)
			+ (sextet_c << 1 * 6)
			+ (sextet_d << 0 * 6);

		if (j < output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
		if (j < output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
		if (j < output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
	}

	/*
	for (int i = 0; i < output_length; ++i) {
		if (i % 16 == 0 && i != 0) printf("\n");
		printf("%02X ", (unsigned char)decoded_data[i]);
	}
	*/
	base64_cleanup();
	return decoded_data;
}

void build_decoding_table() {

	decoding_table = malloc(256);

	for (int i = 0; i < 64; i++)
		decoding_table[(unsigned char)encoding_table[i]] = i;
}


void base64_cleanup() {
	free(decoding_table);
}


void writeByteArrayToFile(char * bufferToWrite, unsigned long fileSize, unsigned int type) {
	FILE * outputFile = NULL;
	//Open the image file in binary mode
	if (type == PNG) outputFile = fopen("./out.png", "wb");
	else if (type == JPG) outputFile = fopen("./out.jpg", "wb");

	if (!outputFile) {
		fprintf(stderr, "Couldn't open the file. Check the path\n");
		exit(1);
	}

	//write the image byte array to file
	fwrite(bufferToWrite, sizeof(unsigned char), fileSize, outputFile);
	//close the file pointer
	fclose(outputFile);
}

void printInputBuffer(int bufferSize, unsigned char * image_buffer) {
	int i = 0;
	while (i < bufferSize) {
		printf("%02X ", (unsigned char)image_buffer[i]);
		i++;
		if (!(i % 16)) printf("\n");
	}
	printf("\n");
}