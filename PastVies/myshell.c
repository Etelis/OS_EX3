#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

// array of pids - as stated 100 commands could be executed, pid may occur twice in the array.
pid_t pids[100];

// keep track of all commands preformed.
char history[100][100];

/**
 * @brief history_cmd - will iterate till the given index and print each history command with the coresponding PID.
 * 
 * @param history_index - an index for current history capacity 
 */
void hisotry_cmd(int history_index){
    int i=0;
    for (; i <= history_index; i++){
        printf("%d %s\n", pids[i], &history[i]);
    }
}

/**
 * @brief main
 * 
 * @param argc 
 * @param argv - all paths provided by the CLI should be appended to PATH
 * @return int 
 */
int main(int argc, char *argv[]) {
    char* builtin_cmd[3] = {"history", "cd", "exit"};
    // space for current command with all it's arguments before tokening.
    char current_command[100];
    // arguments after splitting.
    char* command_arguments[100];
    char *token;
    const char delimiter[2] = " ";
    // indexs to keep track of occupation of both arrays.
    int args_index = 0, history_index = 0;

    // for each arument sent append to PATH.
    for(int i = 1; i < argc; i++){
        if( setenv ("PATH", strcat(strcat(getenv("PATH"), ":"), argv[i]), 1) == -1)
            perror("setenv failed");
    }

    // as stated in the excersice, max amount of iterations are 100.
    for(; history_index < 100; history_index++) {
        args_index = 0;
        printf("$ ");
        fflush(stdout);

        // scan input command, do not stop at spaces.
        scanf("%[^\n]%*c", current_command);

        // save command in history before tokenising.
        strcpy(history[history_index], current_command);
        token = strtok (current_command, delimiter);

        // strtok.
        while (token != NULL){
            command_arguments[args_index++] = token;
            token = strtok (NULL, delimiter);
        }

        // execvp expects null at the end of the args arr.
        command_arguments[args_index] = NULL;

        // builtin command_1 = history.
        if ( !strcmp(builtin_cmd[0], command_arguments[0] )){
            pids[history_index] = getpid();
            hisotry_cmd(history_index);
        }

        // builtin command_2 = cd.
        else if ( !strcmp(builtin_cmd[1], command_arguments[0] )){
            pids[history_index] = getpid();
            if (chdir(command_arguments[1]) == -1)
                perror("chdir failed");
        }
        
        // builtin command_3 = exit.
        else if ( !strcmp(builtin_cmd[2], command_arguments[0] )){
            pids[history_index] = getpid();
            break;
        }


        // otherwise - fork current proccess, execvp given command with all args, wait on child proccess.
        else {
            pids[history_index] = fork();
            if (pids[history_index] < 0){
                perror("fork failed");
            }

            if (pids[history_index] == 0){
                if( execvp(command_arguments[0], command_arguments) == -1)
                    perror("exec failed");
            }

            if(pids[history_index]){
                if( wait(NULL) == -1)
                    perror("wait failed");
            }
        }
    }   
}