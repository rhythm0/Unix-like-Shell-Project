#include "shell.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

const int MAX_LINE = 1024;
const int MAX_JOBS = 16;
const int MAX_HISTORY = 10;

msh_t *alloc_shell(int max_jobs, int max_line, int max_history) {

    msh_t *shell = malloc(sizeof(msh_t));
    if (!shell) return NULL;

    shell->max_jobs = max_jobs ? max_jobs : MAX_JOBS;
    shell->max_line = max_line ? max_line : MAX_LINE;
    shell->max_history = max_history ? max_history : MAX_HISTORY;

    return shell;
}


char *parse_tok(char *line, int *job_type) {
    static char *pointer = NULL;

    if (line != NULL) {
        pointer = line;
    }

    if (*pointer == '\0' || pointer == NULL) {
        *job_type = -1;
        return NULL;
    }

    char *job = pointer; // Keep track of the place after the last call
    bool has_cmd = false; 
    char *last_non_space = NULL;

    while (*pointer != '\0') {

        if (*pointer == '&' || *pointer == ';') {
            *job_type = (*pointer == ';') ? 1 : 0;
            *pointer = '\0';
            has_cmd = true;
            pointer++;
            return job;
        }

        if (*pointer != ' ') {
            last_non_space = pointer; // Update the last non-space position
        }
        pointer++;
    }

    if (*pointer == '\0') {
        if (last_non_space != NULL) {
            *job_type = 1;
            pointer = last_non_space + 1; 
            return job;
        } else {
            *job_type = -1;
            return NULL;
        }
    }

    if (!has_cmd) {
        *job_type = 1;
    }
    return job;
}


char **separate_args(char *line, int *argc, bool *is_builtin) {
    *argc = 0; // Reset argument count
    *is_builtin = false; // Assume command is not built-in initially

    if (line == NULL || *line == '\0') {
        return NULL; // No input provided
    }

    // Copy line to a modifiable buffer
    char *line_copy = strdup(line);
    if (!line_copy) {
        exit(EXIT_FAILURE);
    }

    // First pass: count the number of arguments
    char *temp = line_copy;
    char *token = strtok(temp, " ");
    while (token) {
        (*argc)++;
        token = strtok(NULL, " ");
    }

    if (*argc == 0) {
        free(line_copy); // Clean up
        return NULL; // No arguments found
    }

    // Allocate space for arguments array + NULL terminator
    char **argv = calloc(*argc + 1, sizeof(char*));
    if (!argv) {
        free(line_copy); // Clean up
        exit(EXIT_FAILURE);
    }

    // Second pass: populate the arguments array
    strcpy(line_copy, line); // Reset line_copy for tokenization
    int i = 0;
    token = strtok(line_copy, " ");
    while (token) {
        argv[i++] = strdup(token);
        token = strtok(NULL, " ");
    }
    argv[i] = NULL; // NULL-terminate the array

    free(line_copy); // No longer needed
    return argv;
}

int evaluate(msh_t *shell, char *line) {
    // Ignore lines starting with '#'
    char *trimmed_line = line;
    while (*trimmed_line == ' ' || *trimmed_line == '\t') trimmed_line++; // Skip leading whitespace
    if (*trimmed_line == '#' || *trimmed_line == '\0') {
        return 0; // Ignore comment or empty lines
    }


    if (strlen(line) > shell->max_line) {
        printf("error: reached the maximum line limit.\n");
        return 0;
    }

    char *job;
    int type;
    job = parse_tok(line, &type);
    while (job != NULL) {
        int argc;
        bool is_builtin;
        char **argv = separate_args(job, &argc, &is_builtin);

        // Check for "exit" command
        if (argc > 0 && strcmp(argv[0], "exit") == 0) {
            // Free argv and return a special value to indicate shell should exit
            for (int i = 0; i < argc; i++) {
                free(argv[i]);
            }
            free(argv);
            return 1; // Indicate to exit the shell
        }

        // Print the job and its argc value
        if (argv != NULL) {
            for (int i = 0; i < argc; i++) {
                printf("argv[%d]=%s\n", i, argv[i]);
            }
            printf("argc=%d\n", argc);

            // Free the allocated argv array
            for (int i = 0; i < argc; i++) {
                free(argv[i]);
            }
            free(argv);
        }
        job = parse_tok(NULL, &type); // Continue to the next job
        
    }

    return 0; // Indicate to continue running the shell
}

void exit_shell(msh_t *shell) {
    if (shell != NULL) {
        // Free any dynamically allocated fields inside shell here
        // Example: if (shell->some_dynamic_field) free(shell->some_dynamic_field);

        free(shell);
    }
}