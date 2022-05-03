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

#define TEMP_1 "tempfile1.txt"
#define TEMP_2 "tempfile2.txt"
#define SIZE 150
#define ERROR "errors.txt"


/**
 * @brief cleaup at exit function.
 * 
 * @param fd_1 - fd to close.
 * @param fd_2 - fd to close.
 */
void cleanup(int fd_1, int fd_2){
    close(fd_1);
    close(fd_2);
}

/**
 * @brief tolower - receive character and return the lowecase.
 * 
 * @param character - the character.
 * @return int - return the modified character.
 */
int tolower (int character){
    switch (character)
    {
        case 65 ... 90:
            return character + 32;

        default:
            return character;
    }
}

/**
 * @brief - This is a read warp function in order to remove all spaces and new line from file.
 *
 * @param fd - the file descriptor.
 * @param buf - the buffer which the modified data will be appended to.
 * @return int - the number of bytes written into the buffer.
 */
int readWarp(int fd, char* buf){
    int countRead = 0, actualRead=0, i = 0;
    char temp_buf[SIZE];
    // read from file into temp_buf
    countRead = read(fd, &temp_buf, SIZE);

    if (countRead < 0){
        perror("Error in: read");
        exit(-1);
    }

    // iterate on temp_buf extract only letters and numbers into buf.
    for(; i < countRead; i++){
        if(temp_buf[i] != ' ' && temp_buf[i] != '\n'){
            actualRead++;
            *(buf++) = tolower(temp_buf[i]);
        }
    }

    // return number of bytes written into the buffer.
    return actualRead;
}

/**
 * @brief - This function will receive two paths and check if both text files are equal.
 *
 * @param file_1 - location of file 1.
 * @param file_2 - location of file 2.
 * @return int - 1 if files are equal, 0 otherwise, -1 on error.
 */
int compareIdentical(char *file_1, char *file_2){
    char buffer1[SIZE] ={'\0'}, buffer2[SIZE] = {'\0'};
    int countRead1, countRead2, fd_1, fd_2;

    // open both files provided as args.
    fd_1 = open(file_1, O_RDONLY);
    fd_2 = open(file_2, O_RDONLY);

    if(fd_1 < 0 || fd_2 < 0){
        perror("Error in: open\n");
        cleanup(fd_1, fd_2);
        exit(-1);
    }

    do
    {
        countRead1 = read(fd_1, &buffer1, SIZE);
        countRead2 = read(fd_2, &buffer2, SIZE);

        if (countRead1 < 0 || countRead2 < 0){
            perror("Error in: read");
            cleanup(fd_1, fd_2);
            exit(-1);
        }

        if (strcmp(buffer1, buffer2)) return 0;

    } while (countRead1 > 0 && countRead2 > 0);


    return 1;
}

/**
 * @brief This function will check if files are similar to each other.
 *
 * @param file_1 - location of file 1.
 * @param file_2 - location of file 2.
 * @return int - 1 if files are equal, 0 otherwise, -1 on error.
 */
int compareSimilar(char *file_1, char* file_2){

    char buffer[SIZE] = {'\0'};
    int fd_1, fd_2, tempFile1, tempFile2, countFile, result;

    // open two new files, "tempfile1.txt", "tempfile2.txt" to store local changes.
    tempFile1 = open(TEMP_1, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IXGRP);
    tempFile2 = open(TEMP_2, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IXGRP);

    fd_1 = open(file_1, O_RDONLY);
    fd_2 = open(file_2, O_RDONLY);

    if(tempFile1 < 0 || tempFile2 < 0 || fd_1 < 0 || fd_2 < 0){
        perror("Error in: open");
        cleanup(fd_1, fd_2);
        cleanup(tempFile1, tempFile2);
        exit(-1);
    }

    // use read warp and write returned content into file.
    while((countFile = readWarp(fd_1, buffer)) > 0) {        
        if((write(tempFile1, buffer, countFile)) < 0){
            exit(-1);
        }
    }

    // use read warp and write returned content into file.
    while((countFile = readWarp(fd_2, buffer)) > 0){
        if((write(tempFile2, buffer, countFile)) < 0){
            exit(-1);
        }
    }

    // close both fd of temp files.
    cleanup(fd_1, fd_2);
    cleanup(tempFile1, tempFile2);

    // compare if after modification files are identical.
    return compareIdentical(TEMP_1, TEMP_2);

    return result;

}

/**
 * @brief cleanup function.
 *
 */
void tempRemoval(){
    remove(TEMP_1);
    remove(TEMP_2);
}

int main(int argc, char *argv[]) {
    // define an exit protocol.
    atexit(tempRemoval);

    // check if both files are equal.
    if(compareIdentical(argv[1], argv[2])){
        return 1;
    }

    // check if both files are similar.
    if (compareSimilar(argv[1], argv[2])){
        return 3;
    }

    // o.w
    return 2;

}