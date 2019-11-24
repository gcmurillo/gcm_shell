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
#define KYEL   "\x1B[33m"
#define LINESIZE 64
#define clear() printf("\033[H\033[J") 

char *my_read_line(void) {
    char *line = NULL;  
    size_t bufsize = 0;  
    getline(&line, &bufsize, stdin);
    return line;
}


char **my_split_line(char *line) {

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

int my_exec(char **args) {

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

void help() {
    puts(
        KYEL"\n***********" KBLU "GCM SHELL" KYEL "*************\n"
        KYEL"\n*****" KBLU "OS COURSE 2019 | ESPOL" KYEL"******\n"
        KYEL"\n*********************************\n" KRESET
        "\n List of commands supported:"
        "\n-cd"
        "\n-exit"
        "\n-help"
        "\n-iwc -> beautiful version of wc"
        "\n-All general UNIX commands"
    );
}

int iwc(char **args) {

    int i = 1;
    int w = 0;
    int l = 0;
    int file = 0;
    char* file_name;

    do {
        
        if (i > 3) {
            printf(KRED "iwc: to many arguments\n");
            return -1;
        }

        if (args[i] != NULL) {
            if (strchr(args[i], '-')) {
                
                if (strchr(args[i], 'w')) {
                    w++;
                } else if (strchr(args[i], 'l')) {
                    l++;
                } else {
                    printf(KRED "iwc: Unknown arg %s\n", args[i]);
                    return -1;
                }

            } else {
                if (!file) {
                    file_name = args[i];
                    file++;
                } else {
                    printf(KRED "iwc: Only a file name allowed\n");
                    return -1;
                }
                
            }
        } else {
            printf(KRED "iwc: what about the file name and args? :(\n");
            return -1;
        }
        
        i++;

    } while (args[i] != NULL);

    if (!file) {
        printf(KRED "iwc: where is the file?\n");
        return -1;
    }
    printf("\n%s \n", file_name);
    
    printf("%d - %d\n", w, l);

}

int handleBuiltin(char **args) {

    int n_builtin = 4, i, p_arg = 0;
    char* builtin_list[n_builtin];  // list of builtin commands

    builtin_list[0] = "exit";
    builtin_list[1] = "cd";
    builtin_list[2] = "help";
    builtin_list[3] = "iwc";

    for (i = 0; i < n_builtin; i++) {
        if (strcmp(args[0], builtin_list[i]) == 0) {
            p_arg = i + 1;
            break;
        }
    }

    switch (p_arg)
    {
    case 1:
        printf(KYEL "Good bye ;) \n");
        exit(0);
        break;
    
    case 2:
        chdir(args[1]);
        return 1;

    case 3:
        help();
        return 1;

    case 4:
        iwc(args);
        return 1;

    default:
        break;
    }

    return 0;

}


void my_loop(void) {
    
    char *line;
    char **tokens;
    int status;
    char cwd[1024];

    do {

        getcwd(cwd, sizeof(cwd)); // get current directory
        printf(KCYN "SO|201411870|gcm_sh:" KYEL "%s" KCYN ">> " KRESET, cwd);
        line = my_read_line();  // get user line
        tokens = my_split_line(line);
        
        if (handleBuiltin(tokens)) {
            status = 1;
        } else {
            status = my_exec(tokens);
        }

        free(line);
        free(tokens);

    } while (status);

}


int main(int argc, char **argv) {

    clear();
    printf(KYEL "*********************************\n");
    printf(KYEL "*********************************\n");
    printf(KYEL "*********************************\n");
    printf(KBLU "************GCM SHELL************\n");
    printf(KBLU "     * USE AT YOUR OWN RISK *    \n");
    printf(KYEL "*********************************\n");
    printf(KYEL "*********************************\n");
    printf(KYEL "*********************************\n");
    printf("\n\n");

    my_loop();
    return 0;

}