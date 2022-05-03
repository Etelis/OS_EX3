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
#include <dirent.h>



#define SIZE 150
#define COMP_PATH "./comp.out"
#define TEMP_C_OUTPUT "output.txt"

// int parseSpaces(int fd, char *arg){
//     int currentRead, index = 0;
//     char buf[SIZE];

//     currentRead = read(fd, &buf, SIZE);

//     while (buf[index] != ' ') {
//         arg[index] = buf[index];
//         index++;
//     }

//     return ++index;
// }

int parseLines(int fd, char *line){
    int currentRead, index = 0;
    char buf[SIZE];

    currentRead = read(fd, &buf, SIZE);

    while (buf[index] != '\n') {
        line[index] = buf[index];
        index++;
    }

    return ++index;
}

int compareOutPuts(char* currectOutput){
    pid_t pid;
    int status;

    pid = fork();

    if (pid){
        wait(&status);
        switch (WEXITSTATUS(status))
        {
        case 1:
            return 6;
            break;

        case 2:
            return 5;
            break;

        case 3:
            return 4;
            break; 

        default:
            break;
        }
    }

    else{
        printf("Will exec now for COMP.\n");
        execlp(COMP_PATH, currectOutput, TEMP_C_OUTPUT, NULL);
    }

}

int runCFile(){
    printf("WIll run a.out\n");
    pid_t pid;
    char *fileName = "a.out";

    int status;

    pid = fork();

    if (pid){
        wait(&status);
        // check timeout here...
        return 1;
    }

    else{
        printf("Starting exec to a.out \n");
        execlp(fileName, NULL);
    }

}


int isCFIle(char *fileName){
    char *extention = ".c", *extention_pos = fileName + (strlen(fileName) - 2);
    if (strcmp(extention, extention_pos)) return 0;
    return 1;
}

int findCompileRun(char *dirName, char* output){

    pid_t pid;
    int status, outputFile;
    DIR *dip;
    struct dirent *dit;
    struct stat stat_dir;

    outputFile = open(TEMP_C_OUTPUT, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IXGRP);
    lseek(0, 0, SEEK_SET);

    dup2(outputFile, 1);
    close(outputFile);


    if ((dip = opendir(dirName)) == NULL){
        printf("Error in: opendir");
        return -1;
    }

    chdir(dirName);
    while((dit = readdir(dip)) != NULL){
        stat(dit->d_name, &stat_dir);
        if(S_ISREG(stat_dir.st_mode)){
            printf("%s\n", &dit->d_name);
            pid = fork();
            if (pid){
                wait(&status);
                if (!WIFEXITED(status)) return 2;
                printf("No compile error.\n");

                runCFile();

                printf("a.out was ran, will compare files now\n");
                return compareOutPuts(output);
            }

            else {
                printf("starting exec...\n");
                execlp("gcc", "-w", "-o", "a.out", dit->d_name);
            }
        } 
    }

    return 1;
    
}

int main(int argc, char *argv[]) {
    int config, i = 0, eol_index = 0, inputFile;
    char lines[3][SIZE], basewd[SIZE], cwd[SIZE]; 
    DIR *outerDir;
    struct dirent *dit;
    struct stat stat_dir;

    config = open(argv[1], O_RDONLY);
    getcwd(basewd, SIZE);

    for (; i < 3; i++){
        eol_index += parseLines(config, lines[i]);
        lseek(config, eol_index, SEEK_SET);
    }

    inputFile = open(lines[2], O_RDONLY);
    dup2(inputFile, 0);
    close(inputFile);

    if ((outerDir = opendir(lines[0])) == NULL){
        printf("Error in: opendir");
        return -1;
    }
    
    chdir(lines[0]);
    while((dit = readdir(outerDir)) != NULL){
        lstat(dit->d_name, &stat_dir);
        if(S_ISDIR(stat_dir.st_mode) && strcmp(dit->d_name, ".") && strcmp(dit->d_name, "..")){

            findCompileRun(dit->d_name, lines[3]);
            chdir("../");
        }
    }

}