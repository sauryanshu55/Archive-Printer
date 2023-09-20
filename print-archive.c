#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "print-archive.h"

void print_contents(uint8_t* data, size_t size);

int main(int argc, char** argv) {
  // Make sure we have a file input
  if (argc != 2) {
    fprintf(stderr, "Please specify an input filename.\n");
    exit(1);
  }

  // Try to open the file
  FILE* input = fopen(argv[1], "r");
  if (input == NULL) {
    perror("Unable to open input file");
    exit(1);
  }

  // Seek to the end of the file so we can get its size
  if (fseek(input, 0, SEEK_END) != 0) {
    perror("Unable to seek to end of file");
    exit(2);
  }

  // Get the size of the file
  size_t size = ftell(input);
  // printf("%ld\n",size);
  // Seek back to the beginning of the file
  if (fseek(input, 0, SEEK_SET) != 0) {
    perror("Unable to seek to beginning of file");
    exit(2);
  }

  // Allocate a buffer to hold the file contents. We know the size in bytes, so
  // there's no need to multiply to get the size we pass to malloc in this case.
  uint8_t* data = malloc(size);

  // Read the file contents
  if (fread(data, 1, size, input) != size) {
    fprintf(stderr, "Failed to read entire file\n");
    exit(2);
  }

  // Make sure the file starts with the .ar file signature
  if (memcmp(data, "!<arch>\n", 8) != 0) {
    fprintf(stderr, "Input file is not in valid .ar format\n");
    exit(1);
  }

  // Call the code to print the archive contents
  print_contents(data, size);

  // Clean up
  free(data);
  fclose(input);

  return 0;
}

void get_header_metadata(AR_HEADER *header_metadata, uint8_t* data, int cur_pos){
  int fname_len=0;
  for (int i=0;i<16;i++){
    if (data[i]!=47){
      fname_len++;
    } 
    else{
      break;
    }
  }
  
  header_metadata->file_identifier = (char*)malloc(fname_len + 1);
  for (int i=0;i<fname_len;i++){
    header_metadata->file_identifier[i]=(char)data[i];
  }
  header_metadata->file_identifier[fname_len] = '\0';

  int fsize_len=0;
  for (int i=48;i<58;i++){
    if (data[i]!=32){
      fsize_len++;
    }
    else{
      break;
    }
  }

  char *fsize_s = (char*)malloc(fsize_len + 1);
  for (int i=48;i<48+fsize_len;i++){
    fsize_s[i-48]=data[i];
  }
  fsize_s[fsize_len] = '\0';
  header_metadata->file_size=atoi(fsize_s);
  free(fsize_s);
}
/**
 * This function should print the name of each file in the archive followed by its contents.
 *
 * \param data This is a pointer to the first byte in the file.
 * \param size This is the number of bytes in the file.
 */
void print_contents(uint8_t* data, size_t size) {
  int cur_pos=8;
  data+=cur_pos;
  
  do{
    AR_HEADER *header_metadata=malloc(sizeof(AR_HEADER)); 
    get_header_metadata(header_metadata,data, cur_pos);
    printf("%s\n", header_metadata->file_identifier);
    for (int i=60;i<60+header_metadata->file_size;i++){
      printf("%c",data[i]);
    }
    data=data+60+header_metadata->file_size-1;
    cur_pos=cur_pos+60+header_metadata->file_size;
    if (header_metadata->file_size%2==1){
      data++;
      cur_pos++;
    }
    free(header_metadata->file_identifier);
    free(header_metadata);
    data++;
    printf("\n");
  }while(cur_pos<size-1);
}
