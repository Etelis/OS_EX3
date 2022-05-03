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

#define TEMP "grep_temp_output.txt"
#define TEMP_1 "tempfile1.txt"
#define TEMP_2 "tempfile2.txt"
#define SIZE 150
#define CASEINSENSETIVE 0
#define CASESENSATIVE 1
#define ERROR "error.txt"

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
    
    // iterate on temp_buf extract only letters and numbers into buf.
    for(; i < countRead; i++){
        if(temp_buf[i] != ' ' && temp_buf[i] != '\n'){
            actualRead++;
            *(buf++) = temp_buf[i];
        }
    }

    // return number of bytes written into the buffer.
    return actualRead;
}

/**
 * @brief - This function will receive two paths and check if both text files are equal.
 * 
 * @param file1Loc - location of file 1.
 * @param file2Loc - location of file 2.
 * @param isCaseSens - should case be checked?
 * @return int - 1 if files are equal, 0 otherwise, -1 on error.
 */
int compareIdentical(char *file1Loc, char *file2Loc, int isCaseSens){
    // declaration of two argument lists, one for sensative cases, and the second for insenstative cases.
    char *arg_list_casesens[] = {"-F", "-f", file1Loc, file2Loc, NULL};
    char *arg_list_caseinsens[] = {"-F", "-i", "-f", file1Loc, file2Loc, NULL};

    // determine args list based on isCaseSens.
    char **arg_list = (isCaseSens == CASESENSATIVE) ? arg_list_casesens : arg_list_caseinsens;

    int firstFile, secondFile, tempFile;
    firstFile = open(file1Loc, O_RDONLY);
    secondFile = open(file2Loc, O_RDONLY);

    struct stat stat_temp;
    pid_t pid;

    // "grep_temp_output.txt" - will redirrect stout of grep into the file to verify if files are identical.
    tempFile = open(TEMP, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IXGRP);

    pid = fork();
    
    if (pid < 0){
        printf("Error in: fork\n");
        return -1;
    }

    if (pid) {
        wait(NULL);

        // get stat of the file to check if empty.
        if (stat(TEMP, &stat_temp) == -1){
            printf("Error in: stat\n");
            return -1;
        }

        // close not needed fds.
        close(tempFile);
        close(firstFile);
        close(secondFile);

        // if TEMP file is empty meaning files are not identical., otherwise they are.
        if (stat_temp.st_size != 0) return 1;
        return 0;
    }

    // child proccess will replace stdout with tempFile and execute grep.
    else {
        dup2(tempFile, 1);

        if (execvp("grep", arg_list) < 0)
            printf("Error in: execvp\n");
    }

}

/**
 * @brief This function will check if files are similar to each other.
 * 
 * @param file1Loc - location of file 1.
 * @param file2Loc - location of file 2.
 * @return int - 1 if files are equal, 0 otherwise, -1 on error.
 */
int compareSimilar(char *file1Loc, char *file2Loc){

    char buffer[SIZE];
    int firstFile, secondFile, tempFile1, tempFile2, countFile;

    // open both files provided as args.
    firstFile = open(file1Loc, O_RDONLY);
    secondFile = open(file2Loc, O_RDONLY);
    
    // open two new files, "tempfile1.txt", "tempfile2.txt" to store local changes.
    tempFile1 = open(TEMP_1, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IXGRP);
    tempFile2 = open(TEMP_2, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IXGRP);

    if(firstFile < 0 || secondFile < 0 || tempFile1 < 0 || tempFile2 < 0){
        printf("Error in: open\n");
        return -1;
    }

    // use read warp and write returned content into file.
    while((countFile = readWarp(firstFile, buffer)) > 0) write(tempFile1, &buffer, countFile);

    if (countFile < 0){
        printf("Error in: read");
        return -1;
    }

    // use read warp and write returned content into file.
    while((countFile = readWarp(secondFile, buffer)) > 0) write(tempFile2, &buffer, countFile);
    if (countFile < 0){
        printf("Error in: read");
        return -1;
    }

    // close both fd of temp files.
    close(tempFile1);
    close(tempFile2);

    if(compareIdentical(TEMP_1, TEMP_2, CASEINSENSETIVE)) return 1;

    return 0;

} 

/**
 * @brief cleanup function.
 * 
 */
void tempRemoval(){
    // remove(TEMP);
    // remove(TEMP_1);
    // remove(TEMP_2);
}

int main(int argc, char *argv[]) {

    // change sdterr to error.txt
    int error = open(ERROR, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IXGRP);
    dup2(error, 2);
    close(error);

    // define an exit protocol.
    atexit(tempRemoval);

    if(compareIdentical(argv[1], argv[2], CASESENSATIVE)){
        printf("arg1: %s, arg2: %s\n", argv[1], argv[2]);
        return 1;
    }

    if (compareSimilar(argv[1], argv[2])){
        return 2;
    }

    return 3;

}