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

/* global variables */
char* text_str;

/* constants */
#define DEBUG
#define ALPHABETS_NUM 256
#define BUFFER_MAX 256
#define ERROR -1
#define SUCCESS 0

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

short get_string_frequency(char* string, int string_len, int freq[]){
    int unique_chars = 0;
    int c;
    for (int i = 0; i < string_len; i++){
        c = string[i];
        if (freq[c] == 0){
            ++unique_chars;
        }
        ++freq[c];
    }
    return unique_chars;
}

// int strcmp_custom(const char* s1, const char* s2)
// {
//     while(*s1 && (*s1==*s2) && (s1!))
//         s1++,s2++;
//     return *(const unsigned char*)s1-*(const unsigned char*)s2;
// }

int suffix_str_compare_func(const void *indexp_1, const void *indexp_2){
    int index_1 = *(int*) indexp_1;
    int index_2 = *(int*) indexp_2;
    char* str_1 = &text_str[index_1];
    char* str_2 = &text_str[index_2];
    return strcmp(str_1, str_2);
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

    /* data structure */
    // char* text_str;
    text_str = read_file(input_path);
    int text_len = strlen(text_str);

    int freq[ALPHABETS_NUM]={0};
    int* suffix_indexes;
    int unique_chars;
    

    #ifdef DEBUG 
        printf("%s\n", text_str); 
        printf("Length: %d\n\n", text_len); 
    #endif

    #ifdef DEBUG
        printf("Starting to encode!\n");
    #endif
    
    unique_chars = get_string_frequency(text_str, text_len, freq);
    #ifdef DEBUG
        printf("Unique chars: %d\n", unique_chars);
        printf("Frequecy:\n");
        for (int i = 0; i < ALPHABETS_NUM; i++){
            if (freq[i] > 0){
                printf("%c: %d\n", (char)i, freq[i]);
            }
        }
    #endif

    /* initialize suffix array */
    #ifdef DEBUG
        printf("\ninitializing suffix array:\n");
    #endif

    FILE *output_file = fopen(output_path, "wb");
    int index;
    
    for (int i = 0; i < ALPHABETS_NUM; i++){    
        if (freq[i] > 0){
        
            suffix_indexes = (int*) malloc(freq[i]*sizeof(int));
            index = 0;
            for (int j = 0; j < ALPHABETS_NUM; j ++){
                if ( (int)text_str[j]== i){
                    suffix_indexes[index++]=j;
                }
            }
            qsort(suffix_indexes, index, sizeof(int), suffix_str_compare_func);
            printf("%c:", (char)i);
            for (int k = 0; k < index; k ++){
                printf(" %d|", suffix_indexes[k]);
            }
            printf("\n");

            char bwt_char;
            for (int k = 0; k < index; k ++){
                printf("-%d", suffix_indexes[k]-1);
                if (suffix_indexes[k] != 0){
                    bwt_char = text_str[suffix_indexes[k]-1];
                } else{
                    bwt_char = text_str[text_len-1];
                }
                printf("->%c\n", bwt_char);
                /*fprintf(output_file, "%c",bwt_char);*/
                /*fflush(stdout);*/
                /*fputc(bwt_char, output_file);*/
            }

            free (suffix_indexes);
        }
        
    }


    /* terminate and free memory */
    fclose (output_file);
    free(text_str);
}
























