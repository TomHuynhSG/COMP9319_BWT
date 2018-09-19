/*
COMP9319 Web Data Compression and Search
Assignment 2: BWT and Search 

Name: Nguyen Minh Thong Huynh
ID: z5170141
*/

/* libraries */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/* constants */
#define DEBUG
#define BUFFER_MAX 256
#define ERROR -1
#define SUCCESS 0

/* data structure */


/* functions */

/* Main Program */
int main(int argc, char **argv){

    if (argc != 5){
		fprintf(stderr, "ERROR: Missing inputs\n");
    }

    char* delimiter = (char*) argv[1];
    char* index_path = (char*) argv[2];
    char* input_path = (char*) argv[3];
    char* output_path = (char*) argv[4];
    
    #ifdef DEBUG
        printf("Delimiter: %c\n", *delimiter);
        printf("Index Path: %s\n", index_path);
        printf("Input Path: %s\n", input_path);
        printf("Output Path: %s\n", output_path);
        printf("Starting to encode!\n");
    #endif

}
























