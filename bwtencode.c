/*
COMP9319 Web Data Compression and Search
Assignment 2: BWT and Search 

Name: Nguyen Minh Thong Huynh
ID: z5170141

Part 1: Encoding BWT 
*/

/* 
Example command:
./bwtencode "|" ./tmp ./dummy.txt ./dummy.bwt
*/

/* libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* global variables */
char* text_str;
char* delimiter;

/* constants */
/*#define DEBUG*/
#define ALPHABETS_NUM 256
#define BUFFER_SIZE 5000000
#define ERROR -1
#define SUCCESS 0

/* functions */

/* read the whole file into memory */
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

/* replace aux extension */
char * replace_aux_extension(char* mystr) {
    char *retstr;
    char *lastdot;
    if (mystr == NULL)
         return NULL;
    if ((retstr = malloc (strlen (mystr) + 1)) == NULL)
        return NULL;
    strcpy (retstr, mystr);
    lastdot = strrchr (retstr, '.');
    if (lastdot != NULL){
        lastdot[1] = 'a';
        lastdot[2] = 'u';
        lastdot[3] = 'x';
    }    
    return retstr;
}

/* get string frequency of bwt */
short get_string_frequency(char* string, int string_len, int freq[]){
    int unique_chars = 0;
    int c;
    int i;
    for (i = 0; i < string_len; i++){
        c = string[i];
        if (freq[c] == 0){
            ++unique_chars;
        }
        ++freq[c];
    }
    return unique_chars;
}

/* custom string comparision function for qsort which has special case for delimiters */
int strcmp_custom(const char* s1, const char* s2){
    while(*s1 && (*s1==*s2)){
        if ((*s1==*delimiter)&&(*s2==*delimiter)){
            if(s1 > s2){
                return 1;
            } else if (s1 < s2){
                return -1;
            } else {
                return 0;
            }
        }
        s1++, s2++;
    }
    return *(const unsigned char*)s1-*(const unsigned char*)s2;
}

/* custom string comparision function for qsort which has special case for delimiters */
int suffix_str_compare_func(const void *indexp_1, const void *indexp_2){
    int index_1 = *(int*) indexp_1;
    int index_2 = *(int*) indexp_2;
    char* str_1 = &text_str[index_1];
    char* str_2 = &text_str[index_2];
    return strcmp_custom(str_1, str_2);
}

/* Main Program */
int main(int argc, char **argv){

    if (argc != 5){
		fprintf(stderr, "ERROR: Missing inputs\n");
    }

    delimiter = (char*) argv[1];

    if (strcmp(delimiter,"\\n")==0){
        #ifdef DEBUG
            printf("Detect delimiter is new line char!\n");
        #endif
        delimiter[0]='\n';
        delimiter[1]='\0';
    }

    if (strcmp(delimiter,"\\t")==0){
        #ifdef DEBUG
            printf("Detect delimiter is tab char!\n");
        #endif
        delimiter[0]='\t';
        delimiter[1]='\0';
    }

    char* index_path = (char*) argv[2];
    char* input_path = (char*) argv[3];
    char* output_path = (char*) argv[4];
    char* aux_path = replace_aux_extension(output_path);

    #ifdef DEBUG
        printf("Delimiter: %s (string)| %c (char)| %d (code)\n", delimiter, *delimiter, *delimiter);
        printf("Index Path: %s\n", index_path);
        printf("Aux Path: %s\n", aux_path);
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
        // printf("%s\n", text_str); 
        printf("Length: %d\n\n", text_len); 
    #endif

    #ifdef DEBUG
        printf("Starting to encode!\n");
    #endif
    
    unique_chars = get_string_frequency(text_str, text_len, freq);
    int i;
    #ifdef DEBUG
        printf("Unique chars: %d\n", unique_chars);
        printf("Frequecy:\n");
        for (i = 0; i < ALPHABETS_NUM; i++){
            if (freq[i] > 0){
                printf("%c: %d\n", (char)i, freq[i]);
            }
        }
    #endif

    #ifdef DEBUG
        printf("Delimiter positions:\n");
    #endif
    int* del_pos = (int*) malloc(freq[(int)(*delimiter)]*sizeof(int));

    int del_index = 0;

    for (i = 0; i < text_len; i++){
        if (text_str[i] == *delimiter){
            del_pos[del_index++]=i;
        };
    }
    #ifdef DEBUG
    for (i = 0; i < del_index; i++){
        printf("%d ", del_pos[i]);
    }
    #endif

    /* initialize suffix array */
    #ifdef DEBUG
        printf("\n\nInitializing suffix array:\n");
    #endif

    FILE *output_file = fopen(output_path, "wb");
    int index;
    int cur_row=0;
    int* aux_row_pos = (int*) malloc(freq[(int)(*delimiter)]*sizeof(int));
    int j;
    int k;
    int t;
    int l;
    char buffer[BUFFER_SIZE];
    int buffer_len=0;
    for (i = 0; i < ALPHABETS_NUM; i++){    
        if (freq[i] > 0){
        
            suffix_indexes = (int*) malloc(freq[i]*sizeof(int));
            index = 0;
            for (j = 0; j < text_len; j ++){
                if ( (int)text_str[j]== i){
                    suffix_indexes[index++]=j;
                }
            }
            qsort(suffix_indexes, index, sizeof(int), suffix_str_compare_func);
            #ifdef DEBUG
                printf("%c:", (char)i);
            for (k = 0; k < index; k ++){
                printf(" %d|", suffix_indexes[k]);
                fflush( stdout );
            }
            #endif

            char bwt_char;
            int bwt_pos;
            for (k = 0; k < index; k ++){
                // #ifdef DEBUG
                //     printf("-%d", suffix_indexes[k]-1);
                // #endif
                if (suffix_indexes[k] != 0){
                    bwt_pos = suffix_indexes[k]-1;
                    bwt_char = text_str[bwt_pos];
                } else{
                    bwt_pos = text_len-1;
                    bwt_char = text_str[bwt_pos];
                }
                if (bwt_char == *delimiter){
                    for (t=0; t<del_index; t++){
                        if (del_pos[t]==bwt_pos){
                            #ifdef DEBUG
                                printf("Del (%d) is at row %d for pos %d in original text\n",t, cur_row, del_pos[t]);
                            #endif
                            aux_row_pos[t]=cur_row;
                            fflush( stdout );
                        }
                    }
                }
                cur_row++;
                // #ifdef DEBUG
                //     printf(" -> %c\n", bwt_char);
                // #endif
                //fputc(bwt_char, output_file);
                buffer[buffer_len++]=bwt_char;
            }
            
            for (l=0; l<buffer_len; l++){
                fputc(buffer[l], output_file);
            }
            buffer_len=0;
            free (suffix_indexes);
        }
        
    }
    #ifdef DEBUG
    printf("\nDeliminator positions in BWT:\n");
    for (i=0; i<freq[(int)(*delimiter)]; i++){
        printf("%d ",aux_row_pos[i]);
    }
    printf("\n");
    #endif

    FILE* aux_file = fopen(aux_path,"wb");
    fwrite(aux_row_pos, sizeof(int), freq[(int)(*delimiter)], aux_file);
    fclose(aux_file);

    /* terminate and free memory */
    fclose (output_file);
    free(text_str);
    return SUCCESS;
}
























