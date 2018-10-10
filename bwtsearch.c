/*
COMP9319 Web Data Compression and Search
Assignment 2: BWT and Search 

Name: Nguyen Minh Thong Huynh
ID: z5170141

Part 2: Searching BWT string
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
#include <stdbool.h>


/* constants */
/*#define DEBUG*/
#define ALPHABETS_NUM 128
#define ERROR -1
#define SUCCESS 0


/* functions */

/* Do not use this function anymore since it is read the whole file into memory which is replace by bwt_fseek function and aux_fseek */
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

/* Optimised read bwt char based on position on input file */
char bwt_fseek(int pos, FILE* f){
    fseek(f, pos, SEEK_SET);
    return (fgetc(f));
}


/* Optimised read delimiters pos based on position on aux file */
int aux_fseek(int pos, FILE* f){
    fseek(f, pos*sizeof(int), SEEK_SET);
    int aux_pos;
    fread(&aux_pos, sizeof(int), 1, f);
    return aux_pos;
}

/* check bwt len */
long bwt_flen(FILE* f){
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  //same as rewind(f);
    return (fsize);
}

/* replace aux extension */
char * replace_aux_extension(char* file_name) {
    char *renamed_name;
    char *lastdot;
    if (file_name == NULL){
        return NULL;
    }
    if ((renamed_name = malloc (strlen (file_name) + 1)) == NULL){
        return NULL;
    }   
    strcpy (renamed_name, file_name);
    lastdot = strrchr (renamed_name, '.');
    if (lastdot != NULL){
        lastdot[1] = 'a';
        lastdot[2] = 'u';
        lastdot[3] = 'x';
    }    
    return renamed_name;
}

/* Do not use this function anymore since it is read the whole file into memory which is replace by bwt_fseek function and aux_fseek */
int* read_aux_file(char *input_path, int num_deli){
    FILE *f = fopen(input_path, "rb");
    int* aux_arr= (int *)malloc(num_deli * sizeof(int));
    fread(aux_arr, sizeof(int), num_deli, f);
    fclose(f);
    return aux_arr;
}

/* get string frequency of bwt */
short get_string_frequency(FILE* bwt_file, int string_len, int* freq){
    int i;
    int unique_chars = 0;
    int c;
    for (i = 0; i < string_len; i++){
        c = bwt_fseek(i,bwt_file);
        if (freq[c] == 0){
            ++unique_chars;
        }
        ++freq[c];
    }
    return unique_chars;
}

/* create c table */
void create_c_table(int c[], int freq[]){
    c[0] = 0;
    int i;
    for(i = 1; i < ALPHABETS_NUM; i++) {
        c[i] = c[i-1] + freq[i-1];
    }
}

/* create occ table */
int create_occ_table(FILE* bwt_file, int bwt_len ,int** occ, int freq[],int checkpoint_skip){
    int i;
    int c;
    
    int* temp = (int *)calloc(ALPHABETS_NUM, sizeof(int));
    int occ_rows=0;
    for (i = 0; i < bwt_len; i++){
        c = bwt_fseek(i, bwt_file); 
        temp[c]+=1;
        
        /* create new entry in occ table every checkpoint_skip numbers of bwt chars */
        if (i%checkpoint_skip==0){
            occ[occ_rows] = (int *)malloc(ALPHABETS_NUM * sizeof(int));
            memcpy(occ[occ_rows], temp, ALPHABETS_NUM * sizeof(int));
            occ_rows++;
        }
    }
    return occ_rows;
}

/* read occ entry based on skipped checkpoints occ table */
int occ_seek(int pos, int c, int** occ, FILE* bwt_file,int checkpoint_skip){
    int bucket = pos/checkpoint_skip;
    int count =  occ[bucket][c];
    int lower_bound_pos = bucket*checkpoint_skip;
    int i;
    int cur_c;
    for (i = lower_bound_pos+1; i <=pos; i++){
        cur_c = bwt_fseek(i, bwt_file);
        if (cur_c == c){
            count++;
        }
    }
    return count;
}

/* which record of pos position of bwt string belongs to */
int which_record(int pos, char* delimiter, FILE* bwt_file, int* c, int** occ, FILE* aux_file, int num_deli, int checkpoint_skip){
    int cur_c = bwt_fseek(pos, bwt_file);
    #ifdef DEBUG
        printf("Initial: %d | %c\n", pos, cur_c);
    #endif
    while( cur_c != (int)*delimiter){
        if ((pos)>=1){
            pos = c[cur_c] + occ_seek(pos - 1, cur_c, occ, bwt_file, checkpoint_skip);
        } else {
            pos = c[cur_c];
        }
        cur_c = bwt_fseek(pos, bwt_file);
        #ifdef DEBUG
            printf("Next: %d | %c\n", pos, cur_c);
        #endif
    }
    int i;
    for (i=0; i<num_deli; i++){
        if (pos == aux_fseek(i,aux_file)){
            if ((i+2)>num_deli){
                return 1;
            } else {
                return i+2;
            }

        }
    }
    return ERROR;
}


/* print record of original record based on record number for -i */
void print_record(int record_no, char* delimiter, FILE* bwt_file, int* c, int** occ, int checkpoint_skip){
    int pos = c[(int)*delimiter] + record_no;
    int cur_c = bwt_fseek(pos, bwt_file);

    char temp[5000];
    int tem_len=0;
    while( cur_c != (int)*delimiter){
        temp[tem_len++]=cur_c;
        if ((pos)>=1){
            pos = c[cur_c] + occ_seek(pos - 1, cur_c, occ, bwt_file, checkpoint_skip);
        } else {
            pos = c[cur_c];
        }
        #ifdef DEBUG
            printf("Position %d | %c because %d (C) + ?\n",pos, cur_c, c[cur_c]);
        #endif
        cur_c = bwt_fseek(pos,bwt_file);
    }
    int i;
    for (i=tem_len-1;i>=0;i--){
        printf("%c", temp[i]);
    }
    printf("\n");
}



/* Main Program */
int main(int argc, char **argv){

    if (argc != 6){
		fprintf(stderr, "ERROR: Missing inputs\n");
    }

    char* delimiter = (char*) argv[1];

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

    char* bwt_path = (char*) argv[2];
    char* index_path = (char*) argv[3];
    char* flag = (char*) argv[4];
    char* search_str = (char*) argv[5];
    char* aux_path = replace_aux_extension(bwt_path);

    #ifdef DEBUG
        printf("Delimiter: %s (string)| %c (char)| %d (code)\n", delimiter, *delimiter, *delimiter);
        printf("Input Path: %s\n", bwt_path);
        printf("Index Path: %s\n", index_path);
        printf("Aux Path: %s\n", aux_path);
        printf("Flag: %s\n", flag);
        printf("Search Str: %s\n\n", search_str);
    #endif

    #ifdef DEBUG
        printf("Reading BWT file!\n");
    #endif
    
    FILE *bwt_file = fopen(bwt_path, "rb");
    long bwt_len = bwt_flen(bwt_file);

    #ifdef DEBUG
        printf("BWT Length: %ld\n\n", bwt_len); 
    #endif

    // int freq[ALPHABETS_NUM]={0};
    int* freq = (int *)calloc(ALPHABETS_NUM, sizeof(int));
    int unique_chars = get_string_frequency(bwt_file, bwt_len, freq);

    int num_deli = freq[(int)(*delimiter)];

    FILE *aux_file = fopen(aux_path, "rb");
    
    int i;
    #ifdef DEBUG
        printf("Num of delimiters: %d\n", num_deli);
        printf("Delimiter pos in BWT:\n");
        for (i = 0; i < num_deli; i++){
                printf("%d ", aux_fseek(i,aux_file));
        }
        printf("\n\n");
    #endif


    #ifdef DEBUG
        printf("Unique chars: %d\n", unique_chars);
        printf("Frequecy:\n");
        for (i = 0; i < ALPHABETS_NUM; i++){
            if (freq[i] > 0){
                printf("%c: %d |", (char)i, freq[i]);
            }
        }
        printf("\n");
    #endif

    int* c = (int *)calloc(ALPHABETS_NUM, sizeof(int));
    create_c_table(c,freq);
    #ifdef DEBUG
        printf("C table:\n");
        for (i = 0; i < ALPHABETS_NUM; i++){
            if (freq[i] > 0){
                printf("%c: %d |", (char)i, c[i]);
            }
        }
        printf("\n");
    #endif


    int checkpoint_skip;

    /* make sure file > 10MB will more less checkpoints to save enough memory */
    if (bwt_len<10000){
        checkpoint_skip = 1000;
    } else{
        checkpoint_skip = 2000;
    }

    int **occ = (int **)malloc( ((bwt_len/checkpoint_skip)+1) * sizeof(int*));
    
    int occ_rows = create_occ_table(bwt_file, bwt_len , occ, freq, checkpoint_skip);
    #ifdef DEBUG
        int j;
        printf("Occ/Rank table:\n");
        printf("Number of rows:%d\n",occ_rows);
        printf("   ");
        for (i = 0; i < ALPHABETS_NUM; i++){
            if (freq[i] > 0){
                printf(" %c", i);
            }
        }
        printf("\n");
        for (i = 0; i < occ_rows; i++){
            printf("%c| ", bwt_fseek(i*checkpoint_skip,bwt_file));
            for (j = 0; j < ALPHABETS_NUM; j++){
                if (freq[j] > 0){
                    printf(" %d", occ[i][j]);
                }
            }
            printf("\n");
        }
    #endif

    free(freq);
    
    if (strcmp(flag, "-i")==0){
        
        #ifdef DEBUG
            printf("-i for %s:\n",search_str);
        #endif
        int record_start= 0;
        int record_end = 0;
        sscanf(search_str,"%d %d",&record_start,&record_end);
        #ifdef DEBUG
            printf("Searching from %d to %d:\n",record_start, record_end);
        #endif
    
        for (i = record_start; i <= record_end; i++){
            print_record(i-1, delimiter, bwt_file, c, occ, checkpoint_skip);
        }
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
                low = c[cur_c] + occ_seek(low - 2, cur_c, occ, bwt_file, checkpoint_skip) + 1; 
            } else {
                low = c[cur_c] + 1;
            }
            high = c[cur_c] + occ_seek(high - 1, cur_c, occ, bwt_file, checkpoint_skip);
            #ifdef DEBUG
                printf("s': %d | e': %d \n", low, high);
            #endif
        }   
        if (strcmp(flag, "-m")==0){
            #ifdef DEBUG
                printf("-m searching for '%s' (length: %ld)\n",search_str,strlen(search_str));
            #endif
            int results = high-low+1;
            #ifdef DEBUG
                printf("-m for %s:\n",search_str);
            #endif
            printf("%d\n",results);
        }

        if ((strcmp(flag, "-a")==0)||(strcmp(flag, "-n")==0)){

            int record_num;

            bool matched_records[num_deli];
            memset(matched_records, 0, sizeof(matched_records));

            for (i=low-1;i<=high-1;i++){
                record_num = which_record(i, delimiter, bwt_file, c, occ, aux_file, num_deli, checkpoint_skip);
                matched_records[record_num-1]= true;
            }
            #ifdef DEBUG
                printf("Matched Results:\n");
                for (i = 0; i < num_deli; i++){
                    if (matched_records[i]== true){
                        printf("%d | ", i+1);
                    }
                }            
                printf("\n\n");
            #endif

            if (strcmp(flag, "-a")==0){
                #ifdef DEBUG
                    printf("-a for %s:\n",search_str);
                #endif

                for (i = 0; i < num_deli; i++){
                    if (matched_records[i]== true){
                        printf("%d\n", i+1);
                    }
                }
            }

            if (strcmp(flag, "-n")==0){
                #ifdef DEBUG
                    printf("-n for '%s'\n",search_str);
                #endif

                int num_matches = 0;
                for (i = 0; i < num_deli; i++){
                    if (matched_records[i]== true){
                        num_matches ++;
                    }
                }
                printf("%d\n", num_matches);
            }
        }

    }
    fclose(bwt_file);
    fclose(aux_file);
    return SUCCESS;
}
