/******************************
 *  @author: Geancarlo Murillo
 * 
 *  @year: 2019
******************************/


#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define KRED  "\x1B[31m"
#define KRESET "\x1B[0m"
#define KBLU   "\x1B[34m"
#define KCYN   "\x1B[36m"
#define LINESIZE 64

char *my_read_line(void) {
    /* 
        return char
    */
    char *line = NULL;  
    size_t bufsize = 0;  
    getline(&line, &bufsize, stdin);
    return line;
}


char **my_split_line(char *line) {

    /*
        @param: line
        @return: array of tokens parsed from line
    */

    int bufsize = LINESIZE;
    int position = 0;
    char **tokens = malloc(bufsize * sizeof(char*)); // init allocation of memory to inputs
    char *pch;  // parsed char

    if (!tokens) {  // if error allocating memory
        fprintf(stdin, KRED "gcm_shell: allocation error");
        exit(1);
    }

    pch = strtok (line," \t\n"); 
    while (pch != NULL) {
        tokens[position] = pch;
        position +=1;

        if (position >= bufsize) {  // if position is major than predefined line size
            bufsize += LINESIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {  // if error reallocating memory
                fprintf(stdin, KRED "gcm_shell: allocation error");
                exit(1);
            }

        }
        pch = strtok (NULL, " \t\n");
    }

    tokens[position] = NULL; // array terminated with NULL element
    return tokens; // return array


}

int my_launch(char **args) {

    pid_t pid, wait_p;
    int status;

    pid = fork();

    if (pid < 0) {  // forking error
        perror(KRED "Creating process");
        return 2;
    }

    if (pid == 0) { // child process 

        if(execvp(args[0], args) == -1) { // execute file, -1 in error
            perror(KRED "error executing");
        }
        return 2;

    }

    while ((wait_p = wait(&status)) != -1) // waiting child process
    {
        // printf("Process %lu returned result: %d\n", (unsigned long) wait_p, status);
    }


    return 1;

}


void my_loop(void) {
    
    char *line;
    char **tokens;
    int status;

    do {
        printf(KCYN "SO|201411870|gcm_sh:>> " KRESET);
        line = my_read_line();  // get user line
        // printf("You typed %s \n", line);
        tokens = my_split_line(line);
        status = my_launch(tokens);

        free(line);
        free(tokens);

    } while (status);

}


int main(int argc, char **argv) {

    my_loop();

    return 0;

}