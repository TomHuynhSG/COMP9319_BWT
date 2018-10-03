/*
COMP9319 Web Data Compression and Search
Assignment 2: BWT and Search 

Name: Nguyen Minh Thong Huynh
ID: z5170141
*/

/* 
Example command:
./bwtsearch '|' ./dummy.bwt dummyIndex -a "in"
./bwtsearch '|' ./dummy.bwt dummyIndex -n "in"
./bwtsearch '|' ./dummy.bwt dummyIndex -m "in"
./bwtsearch '|' ./dummy.bwt dummyIndex -i "1 3"
*/

/* libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* global variables */


/* constants */
#define DEBUG
#define ALPHABETS_NUM 128
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

short get_string_frequency(char* string, int string_len, int* freq){
    int i;
    int unique_chars = 0;
    int c;
    for (i = 0; i < string_len; i++){
        c = string[i];
        if (freq[c] == 0){
            ++unique_chars;
        }
        ++freq[c];
    }
    return unique_chars;
}

void create_c_table(int c[], int freq[]){
    c[0] = 0;
    int i;
    for(i = 1; i < ALPHABETS_NUM; i++) {
        c[i] = c[i-1] + freq[i-1];
    }
}


void create_occ_table(char* bwt_str, int bwt_len ,int** occ, int freq[]){
    int i;
    int c;
    occ[0] = (int *)calloc(ALPHABETS_NUM, sizeof(int));
    c = bwt_str[0];
    occ[0][c]=1;

    for (i = 1; i < bwt_len; i++){
        occ[i] = (int *)malloc(ALPHABETS_NUM * sizeof(int));
        memcpy(occ[i], occ[i-1], ALPHABETS_NUM * sizeof(int));
        c = bwt_str[i];
        occ[i][c]+=1;
    }
}

// function occFast(ch, bwt, loc)
//  { if( loc < 0 ) return 0;
//    var bucket = Math.floor(loc/freqBucketSize);
//    var lo = bucket * freqBucketSize;
//    var count = freqCache[bucket][ch.charCodeAt(0)];
//    for(var j = lo; j < loc; j++ )
//       if( bwt.charAt(j) == ch ) count ++ ;
//    return count;
//  }//occFast(ch,bwt,loc)

/* Main Program */
int main(int argc, char **argv){

    if (argc != 6){
		fprintf(stderr, "ERROR: Missing inputs\n");
    }

    char* delimiter = (char*) argv[1];
    char* bwt_path = (char*) argv[2];
    char* index_path = (char*) argv[3];
    char* flag = (char*) argv[4];
    char* search_str = (char*) argv[5];
    #ifdef DEBUG
        printf("Delimiter: %c\n", *delimiter);
        printf("Input Path: %s\n", bwt_path);
        printf("Index Path: %s\n", index_path);
        printf("Flag: %s\n", flag);
        printf("Search Str: %s\n\n", search_str);
    #endif

    #ifdef DEBUG
        printf("Reading BWT file!\n");
    #endif

    char* bwt_str = read_file(bwt_path);
    int bwt_len = strlen(bwt_str);

    #ifdef DEBUG 
        printf("%s\n", bwt_str); 
        printf("Length: %d\n\n", bwt_len); 
    #endif

    // int freq[ALPHABETS_NUM]={0};
    int* freq = (int *)calloc(ALPHABETS_NUM, sizeof(int));
    int unique_chars = get_string_frequency(bwt_str, bwt_len, freq);

    int i;
    int j;
    #ifdef DEBUG
        printf("Unique chars: %d\n", unique_chars);
        printf("Frequecy:\n");
        for (i = 0; i < ALPHABETS_NUM; i++){
            if (freq[i] > 0){
                printf("%c: %d\n", (char)i, freq[i]);
            }
        }
    #endif

    int* c = (int *)calloc(ALPHABETS_NUM, sizeof(int));
    create_c_table(c,freq);
    #ifdef DEBUG
        printf("C table:\n");
        for (i = 0; i < ALPHABETS_NUM; i++){
            if (freq[i] > 0){
                printf("%c: %d\n", (char)i, c[i]);
            }
        }
    #endif

    int **occ = (int **)malloc(bwt_len * sizeof(int*));
    create_occ_table(bwt_str, bwt_len , occ, freq);

    #ifdef DEBUG
        printf("Occ/Rank table:\n");
        printf("   ");
        for (i = 0; i < ALPHABETS_NUM; i++){
            if (freq[i] > 0){
                printf(" %c", i);
            }
        }
        printf("\n");
        for (i = 0; i < bwt_len; i++){
            printf("%c| ", bwt_str[i]);
            for (j = 0; j < ALPHABETS_NUM; j++){
                if (freq[j] > 0){
                    printf(" %d", occ[i][j]);
                }
            }
            printf("\n");
        }
    #endif

    if (strcmp(flag, "-m")==0){
        #ifdef DEBUG
            printf("-m searching for '%s'\n",search_str);
        #endif
        int low = 1;
        int high = bwt_len-1;
        int pat_index;
        int cur_c;
        for (pat_index = strlen(search_str)-1; low<=high && pat_index>=0; pat_index -- ){
            cur_c = search_str[pat_index];
            #ifdef DEBUG
                printf("s: %d | e: %d | ", low, high);
            #endif
            low = c[cur_c] -1 + occ[low-1][cur_c] + 1;
            high = c[cur_c] -1 + occ[high][cur_c];
            #ifdef DEBUG
                printf("s': %d | e': %d \n", low, high);
            #endif
        }
        int results = high-low+1;
        #ifdef DEBUG
            printf("-m for '%s': %d Results\n",search_str,results);
        #endif

    }


}




