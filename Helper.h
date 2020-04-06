#pragma once
#include <stdio.h>
#include <stdlib.h>

#ifndef BASE64_H_C0CE2A47_D10E_42C9_A27C_C883944E704A
#define BASE64_H_C0CE2A47_D10E_42C9_A27C_C883944E704A

unsigned char *  base64_encode(const unsigned char * data, size_t input_length);	//encoding image byte data (compressed) to base64 string 
void build_decoding_table();
unsigned char * base64_decode(const char * data, size_t input_length);	//decoding base64 string to image byte data (compressed)  
void base64_cleanup();

#endif 

bool is_base64(unsigned char c);
void printInputBuffer(int bufferSize, unsigned char * image_buffer);	//print image byte data (compressed)
void writeByteArrayToFile(char * bufferToWrite, unsigned long fileSize, unsigned int type);	// print image byte data (compressed) to image(.png or .jpg) 
																							// file on disk
