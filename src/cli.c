#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "cli.h"
#include "tired.h"

void print_help(const char *prog_name) {
    printf("Usage: %s [options]\n", prog_name);
    printf("\nOptions:\n");
    printf("  -t, --hold-time <ms>    Set hold time in milliseconds (default: %d)\n", DEFAULT_HOLD);
    printf("  -d, --dim-speed <ms>    Set dim speed in milliseconds (default: %d)\n", DEFAULT_SPEED);
    printf("  -b, --brightness <0-100>    Set the brightness of the display when in dim period (default: %d)\n", DEFAULT_BRIGHTNESS);
    printf("  -c, --command <cmd>     Command to execute before exiting\n");
    printf("  -h, --help              Show this help message\n");
}

const struct option options[] = {
    {"hold-time", optional_argument, NULL, 't'},
    {"dim-speed", optional_argument, NULL, 'd'},
    {"brightness", optional_argument, NULL, 'b'},
    {"command", optional_argument, NULL, 'c'},
    {"help", no_argument, NULL, 'h'},
    {NULL, 0, NULL, 0}};

int cli_parse_options(int argc, char** argv, struct cli_options *vars) {
    
    // Initialize defaults before overwriting.
    vars->hold_time = DEFAULT_HOLD;
    vars->dim_time = DEFAULT_SPEED;
    vars->brightness = DEFAULT_BRIGHTNESS;
    vars->command = NULL;

    int opt;
    char *endptr;
    
    while ((opt = getopt_long(argc, argv, "t:d:b:c:h", options, NULL)) != -1) {
        switch (opt) {
            case 't':
                if (!optarg) {
                    fprintf(stderr, "Error: --hold-time requires an argument\n");
                    return CLI_EXIT_FAIL;
                }
                vars->hold_time = strtol(optarg, &endptr, 10);
                if (*endptr != '\0') {
                    fprintf(stderr, "Error: Invalid value for --hold-time: %s\n", optarg);
                    return CLI_EXIT_FAIL;
                }
                break;
                
            case 'd':
                if (!optarg) {
                    fprintf(stderr, "Error: --dim-speed requires an argument\n");
                    return CLI_EXIT_FAIL;
                }
                vars->dim_time = strtol(optarg, &endptr, 10);
                if (*endptr != '\0') {
                    fprintf(stderr, "Error: Invalid value for --dim-speed: %s\n", optarg);
                    return CLI_EXIT_FAIL;
                }
                break;

            case 'b':
                if (!optarg)
                {
                    fprintf(stderr, "Error: --brightness requires an argument\n");
                    return CLI_EXIT_FAIL;
                }
                vars->brightness = strtol(optarg, &endptr, 10);
                if (*endptr != '\0')
                {
                    fprintf(stderr, "Error: Invalid value for --brightness: %s\n", optarg);
                    return CLI_EXIT_FAIL;
                }
                break;
            case 'c':
                if (!optarg) {
                    fprintf(stderr, "Error: --command requires an argument\n");
                    return CLI_EXIT_FAIL;
                }
                vars->command = strdup(optarg);
                if (!vars->command) {
                    fprintf(stderr, "Error: Memory allocation failed\n");
                    return CLI_EXIT_FAIL;
                }
                break;
                
            case 'h':
                print_help(argv[0]);
                return CLI_EXIT_NORMAL;
                
            default:
                print_help(argv[0]);
                return CLI_EXIT_FAIL;
        }
    }
    
    return CLI_PROCEED;
}

int execute_command(char *command) {
    // Split up command
    char *args[20];
    int i = 0;
    // Tokenize input string
    char *token = strtok(command, " ");
    while (token != NULL && i < 20 - 1) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL; 

    // Begin forking
    int pid = fork();

    if (pid == -1) {
        // Fork failed
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        // New child, execute command.
        execvp(args[0], args);
    }

    int status;
    waitpid(pid, &status, 0);
    return WEXITSTATUS(status);
}