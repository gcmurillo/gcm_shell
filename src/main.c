/******************************
 *  author: Geancarlo Murillo
 *  course: Operative Systems | ESPOL II 2019
 *  year: 2019
 *  description: A tiny command interpreter with builtin functions (cd, iwc)
******************************/


// start header's section
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
// end header's section

// define some colors for UI
#define KRED  "\x1B[31m"
#define KRESET "\x1B[0m"
#define KBLU   "\x1B[34m"
#define KCYN   "\x1B[36m"
#define KYEL   "\x1B[33m"
#define LINESIZE 64  // size of line
#define clear() printf("\033[H\033[J") 


/*
 * Function: my_read_line
 * ----------------------
 * Get the line from stdin, with a maximum size (bufsize)
 * 
 * returns: line from stdin
 */

char *my_read_line(void) {
    char *line = NULL;  
    size_t bufsize = 0;  
    getline(&line, &bufsize, stdin);
    return line;
}


/*
 * Function my_split_line
 * ----------------------
 * Split a given line and return an array of those tokens
 * The last item always is NULL
 * 
 * line: line to split
 * 
 * returns: Array of tokens
 */

char **my_split_line(char *line) {

    int bufsize = LINESIZE;  // maximun size of line
    int position = 0; // init position for pointer in list
    char **tokens = malloc(bufsize * sizeof(char*)); // init allocation of memory to inputs
    char *pch;  // parsed char

    if (!tokens) {  // if error allocating memory
        fprintf(stdin, KRED "gcm_shell: allocation error");
        exit(1);
    }

    pch = strtok (line," \t\n");  // break the str into a series of tokens
    while (pch != NULL) {
        tokens[position] = pch; // assing a position for given token 
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
    tokens[position] = NULL; // array terminated with NULL element - for use execvp in my_exec function
    return tokens; // return array

}

/*
 * Function: my_exec
 * -----------------
 * Execute files using execvp function, given a array terminated with a NULL element.
 * 
 * args: Array of arguments, first arg file to execute.
 * 
 * return: -1 for error
 */

int my_exec(char **args) {

    pid_t pid, wait_p;
    int status;

    pid = fork();

    if (pid < 0) {  // forking error
        perror(KRED "Creating process");
        return -1;
    }

    if (pid == 0) { // child process 
        if(execvp(args[0], args) == -1) { // execute file, -1 in error
            perror(KRED "error executing");
        }
        return -1;
    }

    while ((wait_p = wait(&status)) != -1) // waiting child process
    {
        // waiting
    }
    return 1;

}

/*
 * Function: help
 * --------------
 * Show in stdin a little help message.
 */

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


/* 
 * Function: count_lines
 * ---------------------
 * Thread function for count lines. Given an FD as argument.
 * Using fgets to get line by line.
 */
void *count_lines (void *args) {

    int lines = 0;
    char str[300];
    FILE *fp = (FILE*) args;

    while (fgets(str, sizeof(str), fp)) {
        lines++;
    }

    printf(" %d lines", lines);

}

/*
 * Function: count_words
 * ---------------------
 * Thread function for count words. Given a FD as argument.
 * Use strtok to split the lines and get words to count.
 */

void *count_words (void *args) {

    int words = 0;
    char str[300];
    char *token_words;
    FILE *fp = (FILE*) args;

    while (fgets(str, sizeof(str), fp)) {
       token_words = strtok(str, " \t\n");

        while (token_words != NULL) {
            words++;
            token_words = strtok(NULL, " \t\n");
        }
    }

    printf(" %d words\n", words);    

}

/*
 * Function: iwc
 * -------------
 * Builtin version for wc. Use two flags -w and -l, using threads
 * -w flag prints in stdin the number of words in a given file.
 * -l flag prints in stdin the number of lines in a given file.
 * 
 * Usage: 
 *      The args given are file name and the flags, is validated that 3 are the max number of args
 * 
 * args: Filename and flags
 * 
 */

int iwc(char **args) {


    FILE *fp1, *fp2; // File pointers

    int i = 1, w = 0, l = 0, j = 0, file = 0; // flags, iterators 
    char* file_name; // file to read

    pthread_t lines_thread; // threads 
    pthread_t words_thread;

    // start reading args
    do {
        
        if (i > 3) {  // validate number of args given
            printf(KRED "iwc: to many arguments\n");
            return -1;
        }

        if (args[i] != NULL) {
            if (args[i][0] == '-') { // if arg init with '-'

                j = 1;
                
                while (args[i][j]) { // validate for -wl case of use
                    if (args[i][j] == 'w') { 
                        w++;
                    } else if (args[i][j] == 'l') {
                        l++;
                    } else if (!args[i][1]) {
                        printf(KRED "iwc: Where is the flag w or l?\n");
                        return -1;
                    } else {
                        printf(KRED "iwc: Unknown arg %s\n", &args[i][j]);
                        return -1;
                    }
                    j++;
                }

            } else { // if is not a flag, is a file name
                if (!file) {
                    file_name = args[i];
                    file++;
                } else {
                    printf(KRED "iwc: Only a file name allowed\n");
                    return -1;
                }
                
            }
        } else { // if not args 
            printf(KRED "iwc: what about the file name and args? :(\n");
            return -1;
        }
        
        i++;

    } while (args[i] != NULL);
    // end reading args

    // open file
    if (!file) {
        printf(KRED "iwc: where is the file?\n");
        return -1;
    } else {
        fp1 = fopen(file_name, "r");
        fp2 = fopen(file_name, "r");
        if (!fp1 || !fp2) {
            printf(KRED "error reading file %s\n", file_name);
            return -1;
        } 
    }

    // flags
    if (w == 0 && l == 0) {
        w++;
        l++;
    }
    
    
    printf("\n%s has", file_name);

    if (l > 0) {  // creating and join lines thread

        if (pthread_create(&lines_thread, NULL, count_lines, fp1)) {
            printf(KRED "Error creating count lines thread");
        }

        if(pthread_join(lines_thread, NULL)) {
            printf(KRED "Error joining count lines thread\n");
            return 2;
        }

    }
    if (w > 0) {  // creating and join words thread
        if (pthread_create(&words_thread, NULL, count_words, fp2)) {
            printf(KRED "Error creating count words thread");
        }

        if(pthread_join(words_thread, NULL)) {
            printf(KRED "Error joining count words thread\n");
            return 2;
        }

    }
    printf("\n"); 

    fclose(fp1);  // closing fp
    fclose(fp2);
    return 0;
    
}


/*
 * Function: handleBuiltin
 * -----------------------
 * Handle the interaction of the user with the command line.
 * It manage 5 builtin commands:
 * - exit -> End the command line execution
 * - cd -> call to chdir system call
 * - help -> call help function
 * - iwc -> call iwc function
 * - twc -> show message
 * 
 * args: array of line parsed tokens
 * 
 * returns: 1 if first token is a builtin command, else 0
 */

int handleBuiltin(char **args) {

    int n_builtin = 4, i, p_arg = 0; // constant, and iterators
    char* builtin_list[n_builtin];  // list of builtin commands

    // builtin commands
    builtin_list[0] = "exit";
    builtin_list[1] = "cd";
    builtin_list[2] = "help";
    builtin_list[3] = "iwc";
    builtin_list[4] = "twc";

    // looking in args for a builtin command
    for (i = 0; i < n_builtin; i++) {
        if (strcmp(args[0], builtin_list[i]) == 0) {
            p_arg = i + 1;
            break;
        }
    }

    // cases for each args if find
    switch (p_arg) {
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

        case 5:
            printf("Did you mean iwc?");
            return 1;

        default:
            break;
    }

    return 0;

}

/*
 * Function: my_loop
 * -----------------
 * Main loop of command line program
 */
void my_loop(void) {
    
    char *line; // user input
    char **tokens; // line parsed
    int status; // status for commands execution
    char cwd[1024]; // str for working directory path

    do {

        getcwd(cwd, sizeof(cwd)); // get current directory
        printf(KCYN "SO|201411870|gcm_sh:" KYEL "%s" KCYN "$ " KRESET, cwd);
        line = my_read_line();  // get user line
        tokens = my_split_line(line); // get array of tokens from line
        
        if (handleBuiltin(tokens)) {
            status = 1;
        } else {
            status = my_exec(tokens);
        }

        free(line);
        free(tokens);

    } while (status);

}

/* 
 * Function main
 * -------------
 */

int main(int argc, char **argv) {

    clear();
    printf(KYEL "*********************************\n");
    printf(KYEL "*********************************\n");
    printf(KYEL "*********************************\n");
    printf(KBLU "*           GCM SHELL           *\n");
    printf(KBLU "*      USE AT YOUR OWN RISK     *\n");
    printf(KYEL "*********************************\n");
    printf(KYEL "*********************************\n");
    printf(KYEL "*********************************\n");
    printf("\n\n");

    my_loop();
    return 0;

}