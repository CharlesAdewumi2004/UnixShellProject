#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 80
#define MAX_HISTORY 10  

char history[MAX_HISTORY][MAX_LINE]; 
int history_count = 0; 

void execute_command(char *args[], int background) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    } else if (pid == 0) {
        execvp(args[0], args);
        perror("Execution failed");
        exit(1);
    } else {
        if (!background) {
            wait(NULL);
        }
    }
}

void add_to_history(char *input) {
    if (history_count < MAX_HISTORY) {
        strcpy(history[history_count], input);
        history_count++;
    } else {
        for (int i = 0; i < MAX_HISTORY - 1; i++) {
            strcpy(history[i], history[i + 1]);
        }
        strcpy(history[MAX_HISTORY - 1], input);
    }
}

void print_history() {
    for (int i = history_count - 1; i >= 0; i--) {
        printf("%d: %s\n", i + 1, history[i]);
    }
}

int main() {
    char input[MAX_LINE];
    char *args[MAX_LINE / 2 + 1]; 
    int should_run = 1;

    while (should_run) {
        printf("osh> ");
        fflush(stdout);

        if (!fgets(input, MAX_LINE, stdin)) {
            break;
        }

        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "exit") == 0) {
            should_run = 0;
            break;
        }

        if (strcmp(input, "history") == 0) {
            print_history();
            continue;
        }

        add_to_history(input);

        int i = 0;
        char *token = strtok(input, " ");
        int background = 0;

        while (token != NULL) {
            if (strcmp(token, "&") == 0) {
                background = 1; 
            } else {
                args[i] = token;
                i++;
            }
            token = strtok(NULL, " ");
        }

        args[i] = NULL; 

        if (i > 0) {
            execute_command(args, background);
        }
    }

    return 0;
}
