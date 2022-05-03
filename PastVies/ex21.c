#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#define SIZE 150

int compareCompleteEqual(int firstFile, int secondFile){
    char buffer_1[SIZE];
    char buffer_2[SIZE];
    int diffrent = 0, read_1, read_2;

    while (!diffrent && (read_1 = read(firstFile, buffer_1, SIZE)) > 0 && (read_2 = read(secondFile, buffer_2, SIZE)) > 0){
        if (strcmp(buffer_1, buffer_2)) diffrent = 1;
    }

    if (!diffrent) return 1;
    return 0;
}

int compareSimilar(int firstFile, int secondFile){
    char buffer_1[SIZE], buffer_2[SIZE];
    int diffrent = 0;



} 

int main(int argc, char *argv[]) {
    int firstFile, secondFile;
    struct stat stat_fd1, stat_fd2;

    firstFile = open(argv[1], O_RDONLY);
    secondFile = open(argv[2], O_RDONLY);
    

    if (stat(argv[1], &stat_fd1) == -1){
        perror("stat Failed");
    }

    if (stat(argv[2], &stat_fd2) == -1){
        perror("stat Failed");
    }


    if(stat_fd1.st_size == stat_fd2.st_size && compareCompleteEqual(firstFile, secondFile)) {
        close(firstFile);
        close(secondFile);
        exit(1);
    }






}