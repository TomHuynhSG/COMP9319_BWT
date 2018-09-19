/*
COMP9319 Web Data Compression and Search
Assignment 2: BWT and Search 

Name: Nguyen Minh Thong Huynh
ID: z5170141
*/

/* 
Example command:
./bwtencode "|" ./tmp ./dummy.txt ./dummy.bwt
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
char* read_file(char *input_path){
    FILE *f = fopen(input_path, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  //same as rewind(f);

    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);
    string[fsize] = 0;
    return string;
}

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
        printf("Output Path: %s\n\n", output_path);
    #endif
    
    #ifdef DEBUG
        printf("Reading file!\n");
    #endif
    char* input_s;
    input_s = read_file(input_path);
    #ifdef DEBUG 
        printf("%s\n\n", input_s); 
    #endif

    #ifdef DEBUG
        printf("Starting to encode!\n");
    #endif
    

}
























