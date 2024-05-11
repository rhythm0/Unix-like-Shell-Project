#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "shell.h"

int main(int argc, char *argv[]) {
    int max_history = 0;
    int max_jobs = 0;
    int max_line = 0;

    int opt;

    int option_value;

    while ((opt = getopt(argc, argv, "s:j:l:")) != -1) {

        if (optarg == NULL || optarg[0] == '\0' || sscanf(optarg, "%d", &option_value) != 1 || option_value <= 0)
        {
            printf("usage: msh [-s NUMBER] [-j NUMBER] [-l NUMBER]\n");
            return 1;
        }
    
        switch (opt) {
            case 's':
                max_history = option_value;
                break;
            case 'j':
                max_jobs = option_value;
                break;
            case 'l':
                max_line = option_value;
                break;
            default: /* '?' */
                printf("usage: msh [-s NUMBER] [-j NUMBER] [-l NUMBER]\n");
                exit(EXIT_FAILURE);
        }
    }
    // After processing all options with getopt
    if (optind < argc) {
        // This means there are additional arguments that were not processed by getopt
        printf("usage: msh [-s NUMBER] [-j NUMBER] [-l NUMBER]\n");
        return 1;
    }
    msh_t *shell = alloc_shell(max_jobs, max_line, max_history);

    char *line = NULL;
    long int len = 0;
    long nRead;

    // file redirection
    FILE *input_stream = stdin;
    int is_file_input = !isatty(fileno(stdin));


    do {
        if (is_file_input && feof(stdin)) {
            // If EOF reached and input was from file, switch to standard input
            input_stream = fopen("/dev/tty", "r");
            if (input_stream == NULL) {
                perror("Unable to open terminal for input");
                break;
            }
            is_file_input = 0; // Switch to interactive mode
        }

        printf("msh> "), fflush(stdout);
        nRead = getline(&line, &len, input_stream);

        if (nRead > 0 && line[nRead - 1] == '\n') {
            line[nRead - 1] = '\0'; // Remove newline character
        }

        int should_exit = evaluate(shell, line);

        free(line);
        line = NULL;
        if (should_exit) break;
    } while (nRead != -1);

    if (input_stream != stdin) fclose(input_stream);

    free(line);
    exit_shell(shell);

    return 0;

    
    // OLD VERSION
    // while (printf("msh> "), fflush(stdout), (nRead = getline(&line, &len, stdin)) != -1) {
    //     if (strcmp(line, "exit\n") == 0) break;

    //     if (nRead > 0 && line[nRead - 1] == '\n') {
    //         line[nRead - 1] = '\0'; // Replace '\n' with '\0' to remove it
    //     }

        
    //     // Call evaluate with the current shell state and line
    //     if (evaluate(shell, line) == 1){
    //         break;
    //     }

    //     free(line); // Free the line buffer
    //     line = NULL; // Reset line pointer for next getline call
    // }

    // free(line); 
    // exit_shell(shell); 

    // return 0;
}