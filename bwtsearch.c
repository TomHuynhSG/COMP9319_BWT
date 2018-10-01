/*
COMP9319 Web Data Compression and Search
Assignment 2: BWT and Search 

Name: Nguyen Minh Thong Huynh
ID: z5170141
*/

/* 
Example command:
./bwtencode '|' ./dummy.bwt dummyIndex -a "in"
./bwtencode '|' ./dummy.bwt dummyIndex -n "in"
./bwtencode '|' ./dummy.bwt dummyIndex -m "in"
./bwtencode '|' ./dummy.bwt dummyIndex -i "1 3"
*/

/* libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* global variables */
char* bwt_str;
char* delimiter;

/* constants */
/*#define DEBUG*/
#define ALPHABETS_NUM 256
#define ERROR -1
#define SUCCESS 0


/* Main Program */
int main(int argc, char **argv){

    if (argc != 6){
		fprintf(stderr, "ERROR: Missing inputs\n");
    }

    delimiter = (char*) argv[1];
    char* input_path = (char*) argv[2];
    char* index_path = (char*) argv[3];
    char* flag = (char*) argv[4];
    char* search_str = (char*) argv[5];

    #ifdef DEBUG
        printf("Delimiter: %c\n", *delimiter);
        printf("Index Path: %s\n", index_path);
        printf("Input Path: %s\n", input_path);
        printf("Output Path: %s\n\n", output_path);
    #endif
    
}




