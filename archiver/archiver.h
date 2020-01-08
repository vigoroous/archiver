/*
 * Простейший говноархиватор
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define BUF_SIZE 2048
#define PATH_MAX 260

enum Mode {
    HELP = 1, CREATE, EXTRACT
};

struct meta_data {
    char name[PATH_MAX];
    unsigned long long size;
    char mode;
};

#define DIR_MODE S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH

FILE* create_arch(char* arch_name);

//int write_to_arch(int arch_fd, char* name);

int write_file_to_arch(FILE* arch_fp, char* arch_name, char* file);

//int write_dir_to_arch(int arch_fd, char* dir);

int end_of_arch(FILE* arch_fp, char* arch_name);

int extract_from_arch(char* arch_name);

//int extract_dir(int arch_fd, char* name);

//int extract_file(int arch_fd, struct meta_data header);

long int filesize(FILE* fp);

//To be continued...