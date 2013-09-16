#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h> 
#include <string.h>

/*
 * 
 */
bool 
file_exist(char file_path[])
{
    if (access(file_path, F_OK) != -1)
        return(true);
    else
        return(false);
}

int 
main() {
    char    path[100] = {"/home/eduard/workspace/old-hooky/test/ipsum/"};
    char    *buffer = NULL;
    int     i, n, len;
    time_t  t;
    FILE    *file_pointer = NULL;

    srand((unsigned) time(&t));
    n = (rand() % 4) + 1;

    switch (n){
        case 1:
            strcat(path, "one.txt");
        break;

        case 2:
            strcat(path, "two.txt");
        break;

        case 3:
            strcat(path, "three.txt");
        break;

        case 4:
            strcat(path, "four.txt");
        break;
    }

    if (file_exist(path)){

        file_pointer = fopen(path, "r");
        if (file_pointer == NULL) {
            fprintf(stderr, "Unable to open config file. Check file name and path!\n");
            exit(0);
        }

        fseek(file_pointer, 0, SEEK_END);
        len = ftell(file_pointer);
        rewind(file_pointer);

        buffer = (char *) malloc(len);
        if (buffer == NULL){
            fprintf(stderr, "Failed to creat buffer.");
            exit(0);
        }

        if (fread(buffer, 1, len, file_pointer) != len) {
            fprintf(stderr, "Error reading file.\n");
            exit(0);
        }
        fclose(file_pointer);

        printf("%s", buffer);
    } 
    else {
        fprintf(stderr, "Sorry, target file does not exist.");
    }
    return (0);
}
