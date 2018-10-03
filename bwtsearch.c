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
char* read_string_file(char *input_path){
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

int* read_aux_file(char *input_path, int num_deli){
    FILE *f = fopen(input_path, "rb");
    int* aux_arr= (int *)malloc(num_deli * sizeof(int));
    fread(aux_arr, sizeof(int), num_deli, f);
    fclose(f);
    return aux_arr;
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

int which_record(int pos, char* delimiter, char* bwt, int* c, int** occ, int* deli_pos, int num_deli){
    int cur_c = bwt[pos];
    printf("Initial: %d | %c\n", pos, cur_c);
    while( cur_c != (int)*delimiter){
        if ((pos)>=1){
            pos = c[cur_c] + occ[pos-1][cur_c];
        } else {
            pos = c[cur_c];
        }
        cur_c = bwt[pos];
        printf("Next: %d | %c\n", pos, cur_c);
    }
    int i;
    for (i=0; i<num_deli; i++){
        if (pos== deli_pos[i]){
            if ((i+2)>num_deli){
                return 1;
            } else {
                return i+2;
            }

        }
    }
    return ERROR;
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

    char* bwt_str = read_string_file(bwt_path);
    int bwt_len = strlen(bwt_str);

    #ifdef DEBUG 
        printf("%s\n", bwt_str); 
        printf("Length: %d\n\n", bwt_len); 
    #endif

    // int freq[ALPHABETS_NUM]={0};
    int* freq = (int *)calloc(ALPHABETS_NUM, sizeof(int));
    int unique_chars = get_string_frequency(bwt_str, bwt_len, freq);

    int num_deli = freq[(int)(*delimiter)];
    int* deli_pos = read_aux_file("./bwt_pos.aux", num_deli);
    
    int i;
    int j;
    #ifdef DEBUG
        printf("Num of delimiters: %d\n", num_deli);
        printf("Delimiter pos in BWT:\n");
        for (i = 0; i < num_deli; i++){
                printf("%d ", deli_pos[i]);
        }
        printf("\n\n");
    #endif


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

    
        
    #ifdef DEBUG
        printf("-m searching for '%s' (length: %ld)\n",search_str,strlen(search_str));
    #endif
    if (strcmp(flag, "-i")==0){
        printf("Searching for -i\n");


        
    } else {
        int pat_index = strlen(search_str) - 1;
        int cur_c = search_str[pat_index];
        int low = c[cur_c]+1;
        int high = c[cur_c + 1];
        for (; low<=high && pat_index>=1; pat_index -- ){
            cur_c = search_str[pat_index - 1];
            #ifdef DEBUG
                printf("%c | s: %d | e: %d | ",cur_c, low, high);
            #endif
            if ((low)>=2){
                low = c[cur_c] + occ[low-2][cur_c] + 1;
            } else {
                low = c[cur_c] + 1;
            }
            high = c[cur_c] + occ[high-1][cur_c];
            #ifdef DEBUG
                printf("s': %d | e': %d \n", low, high);
            #endif
        }
        if (strcmp(flag, "-m")==0){
            int results = high-low+1;
            #ifdef DEBUG
                printf("-m for '%s': %d Results\n",search_str,results);
            #endif
        }

        if ((strcmp(flag, "-a")==0)||(strcmp(flag, "-n")==0)){
            int record_num;
            for (i=low-1;i<=high-1;i++){
                record_num = which_record(i, delimiter, bwt_str, c, occ, deli_pos, num_deli);
                deli_pos[record_num-1]=-1;
            }
            printf("Matched record:\n");
            if (strcmp(flag, "-a")==0){
                for (i = 0; i < num_deli; i++){
                    if (deli_pos[i]==-1){
                        printf("%d\n", i+1);
                    }
                }
            }

            if (strcmp(flag, "-n")==0){
                int matches = 0;
                for (i = 0; i < num_deli; i++){
                    if (deli_pos[i]==-1){
                        matches ++;
                    }
                }
                printf("%d\n", matches);
            }
            
        }
    }


}




