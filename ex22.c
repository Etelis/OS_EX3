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
#include <sys/time.h>

#define SIZE 150
#define COMP_PATH "./comp.out"
#define TEMP_C_OUTPUT "output.txt"
#define ERROR "errors.txt"
#define RESULTS "results.csv"

#define NO_C_FILE ",0,NO_C_FILE\n"
#define COMPILATION_ERROR ",10,COMPILATION_ERROR\n"
#define TIMEOUT ",20,TIMEOUT\n"
#define WRONG ",50,WRONG\n"
#define SIMILAR ",75,SIMILAR\n"
#define EXCELLENT ",100,EXCELLENT\n"


/**
 * @brief cleanup onexit function.
 * 
 */
void cleanup(){
    remove(TEMP_C_OUTPUT);
}


/**
 * @brief updateGrade - update results txt by the provided name and grade-code.
 * 
 * @param studentName - the name of the student (folder);
 * @param grade - the grade-code generated.
 */
void updateGrade(char *studentName, int grade){
    char write_buf[SIZE] = {'\0'};

    // append student's name to the buffer.
    strcat(write_buf, studentName);

    // deside the grade by the grade-code.
    switch (grade)
    {
        case 1:
            strcat(write_buf, NO_C_FILE);
            break;

        case 2:
            strcat(write_buf, COMPILATION_ERROR);
            break;

        case 3:
            strcat(write_buf, TIMEOUT);
            break;

        case 4:
            strcat(write_buf, WRONG);
            break;

        case 5:
            strcat(write_buf, SIMILAR);
            break;

        case 6:
            strcat(write_buf, EXCELLENT);
            break;

        default:
            break;
    }

    // write into stdin (results.txt)
    write(1, write_buf, strlen(write_buf));
}


/**
 * @brief compareOutPuts - will compare temporary output of the executed program with the correct provided output.
 * 
 * @param correctOutput - the correctoutput file.
 * @return int - will return grade-code correspondin to the comperation.
 */
int compareOutPuts(char* correctOutput){
    pid_t pid;
    int status;
    char *comp_args[]={COMP_PATH ,correctOutput, TEMP_C_OUTPUT, NULL};

    // fork --- COMPARE STAGE ---
    pid = fork();

    if(pid < 0){
        perror("Error in: fork");
        return 0;
    }

    if (pid){
        wait(&status);

        // by the status returned decide grade-code.
        switch (WEXITSTATUS(status))
        {
            case 1:
                return 6;
                break;

            case 2:
                return 4;
                break;

            case 3:
                return 5;
                break;

            default:
                break;
        }
    }

    else{

        // open error's file.
        int error_fd = open(ERROR, O_WRONLY | O_CREAT | O_APPEND, S_IRWXU | S_IXGRP); 

        if(error_fd < 0){
            perror("Error in: open");
            exit(-1);
            } 

        // configure errors to sdterr.
        dup2(error_fd, 2);
        close(error_fd);

        if (execvp(COMP_PATH, comp_args) < 0){
            perror("Error in: execvp");
            exit(-1);
        }
    }

}

/**
 * @brief runCFile - will run the given c file and store output into output.txt.
 * 
 * @param path - the path to the c file.
 * @return int - 0 if timeout occured, o.w 1.
 */
int runCFile(char *path){
    pid_t pid;
    char outputFile[SIZE], *run_args[]={outputFile, NULL};
    int status;

    // append a.out to the end of the path.
    strcpy(outputFile, path);
    strcat(outputFile, "/a.out");

    pid = fork();

    // fork - RUN THE PROGRAM STAGE.
    if (pid < 0){
        perror("Error in: fork");
        return -1;
    }

    // parent proccess.
    if (pid){
        // get the current system time.
        time_t tic = time(NULL);
        wait(&status);
        // after child proccess has finished get the current system time.
        time_t toc = time(NULL);

        // if execution took more than 5 sec, return 0.
        if ((toc - tic) > 5) return 0;

        // o.w return 1.
        return 1;
    }

    // child proccess.
    else{
        // open the TEMP output file.
        int output_fd = open(TEMP_C_OUTPUT, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IXGRP);
        // open errors txt file.
        int error_fd = open(ERROR, O_WRONLY | O_CREAT | O_APPEND, S_IRWXU | S_IXGRP); 

        if (output_fd < 0 || error_fd < 0){
            perror("Error in: open");
            exit(-1);
        }
        // lseek into the begining of the input file. (stdin)
        if(lseek(0, 0, SEEK_SET) < 0){
            perror("Error in: lseek");
            exit(-1);
        }

        // stdout into outputfile, stderr into error file.
        dup2(output_fd, 1);
        dup2(error_fd, 2);
        close(output_fd);
        close(error_fd);

        if(execvp(outputFile, run_args) < 0 ){
            perror("Error in: execvp");
            exit(-1);
        }
    }

}

/**
 * @brief isCFile - function will check if a given file has an extention of ".c"
 * 
 * @param fileName - name of the given file.
 * @return int - 1 if a c file, otherwise 0.
 */
int isCFile(char *fileName){
    char *extention = ".c", *extention_pos = fileName + (strlen(fileName) - 2);
    if (strcmp(extention, extention_pos)) return 0;
    return 1;
}

/**
 * @brief findCompileRun - will try compiling and running c file inside the folder.
 * 
 * @param dirName - full path to inner dir.
 * @param output - correct output provided.
 * @return int - will return grade code according to operation.
 */
int findCompileRun(char *dirName, char* output){
    pid_t pid;
    int status, output_fd;
    DIR *dip;
    struct dirent *dit;
    struct stat stat_dir;
    char dirWithFile[SIZE], dirWithOutput[SIZE];
    char *gcc_args[] = {"gcc",dirWithFile, "-o", dirWithOutput, NULL };

    // try opening the given folder.
    if ((dip = opendir(dirName)) == NULL){
        perror("Error in: opendir");
        return 0;
    }

    // iterate on folder, try finding an .c file, if no such file return grade-code: 1, otherwise try compiling and running.
    while((dit = readdir(dip)) != NULL){
        // first create appropriate path.
        strcpy(dirWithFile, dirName);
        strcat(dirWithFile, "/");
        strcat(dirWithFile, dit->d_name);

        if(stat(dirWithFile, &stat_dir) < 0){
            perror("Error in: stat");
            return 0;
        }

        // check if file is a regular file, also check if the extention is .c.
        if(S_ISREG(stat_dir.st_mode) && isCFile(dit->d_name)){

            //   COMPILE STAGE: //
            pid = fork();
            
            if (pid < 0){
                perror("Error in: fork\n");
                return 0;
            }

            // parent proccess.
            if (pid){
                wait(&status);

                // if child process exited with abnormal status return grade-code: 2 indicating compile failed.
                if (WIFEXITED( status ) && (WEXITSTATUS( status ) != 0)) return 2;

                // otherwise try running.
                if (!runCFile(dirName)) return 3;

                // after running try comparing generated output with correct provided output.
                return compareOutPuts(output);
            }

            // child proccess.
            if(!pid) {

                // open error's file.
                int error_fd = open(ERROR, O_RDWR | O_CREAT | O_APPEND, S_IRWXU | S_IXGRP); 

                if(error_fd < 0){
                    perror("Error in: open");
                    exit(-1);
                } 

                // configure errors to sdterr.
                dup2(error_fd, 2);
                close(error_fd);

                // assing dirname and desired output accordingly, ex: output file: FULL_PATH\a.out
                strcpy(dirWithOutput, dirName);
                strcat(dirWithOutput, "/a.out");

                // run gcc.
                if((execvp("gcc",gcc_args)) < 0){
                    perror("Error in: execvp");
                    exit(-1);
                }
            }
        }
    }

    return 1;

}

/**
 * @brief main function.
 * 
 * @param argc - argc
 * @param argv - path to config.txt
 * @return int - exit code.
 */
int main(int argc, char *argv[]) {

    // define an exit cleanup function.
    atexit(cleanup);

    int config_fd, output_fd, input_fd, results_fd, i = 0, studentResult;
    char lines[3][SIZE], basewd[SIZE], cwd[SIZE], configwd[SIZE];
    DIR *outerDir;
    struct dirent *dit;
    struct stat stat_dir;

    // get the current working directory.
    getcwd(basewd,SIZE);

    // open config file provided by argument.
    config_fd = open(argv[1], O_RDONLY);

    // create new results file.
    results_fd = open(RESULTS, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IXGRP);

    if(config_fd < 0 || results_fd < 0){
        perror("Error in: open");
        return -1;
    }

    // configure config to stdin to read from file easly.
    dup2(config_fd, 0);

    // configute results to stdout.
    dup2(results_fd, 1);

    // close unused FDs.
    close(config_fd);
    close(results_fd);

    // scan config file for: 1. folder, 2. desired input, 3. desired output.
    for (; i < 3; i++){
        scanf("%s", lines[i]);
    }

    
    // try accessing folder and files, verify they do exists.
    if (access(lines[0], F_OK)){
        perror("Not a valid directory\n");
        return -1;
    }

    if (access(lines[1], F_OK)){
        perror("Input file not exists\n");
        return -1;
    }

    if (access(lines[2], F_OK)){
        perror("Output file not exists\n");
        return -1;
    }

    if ((outerDir = opendir(lines[0])) == NULL){
        perror("Error in: opendir");
        return -1;
    }

    // store current working directory.
    getcwd(basewd, SIZE);
    // move into config folder.
    chdir(lines[0]);
    // store full path to config folder.
    getcwd(configwd, SIZE);
    // go back into base working directory.
    chdir(basewd);

    // change stdin to the input file provided.
    input_fd = open(lines[1], O_RDONLY);
    dup2(input_fd, 0);
    close(input_fd);
    
    // iterate over provided folder and for each folder available enter findComplieRun.
    while((dit = readdir(outerDir)) != NULL){

        // apend d_name into the given folder.
        strcpy(cwd, configwd);
        strcat(cwd, "/");
        strcat(cwd, dit->d_name);

        // receive stats for the currect file/dir to check if indeed it's a dir.
        if((lstat(cwd, &stat_dir)) < 0){
            perror("Error in: stat");
            return -1;
        }

        if(S_ISDIR(stat_dir.st_mode) && strcmp(dit->d_name, ".") && strcmp(dit->d_name, "..")){

            // try compiling and running c file inside given folder, return apropriate code.
            studentResult = findCompileRun(cwd, lines[2]);
            
            // by the given code update results file.
            updateGrade(dit->d_name, studentResult);
        }
    }


}