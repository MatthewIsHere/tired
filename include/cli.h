#ifndef CLI_H
#define CLI_H

#define CLI_PROCEED 0
#define CLI_EXIT_NORMAL -1
#define CLI_EXIT_FAIL -2

struct cli_options {
    unsigned int hold_time;
    unsigned int dim_time;
    unsigned int brightness;
    char * command;
};

int cli_parse_options(int argc, char **argv, struct cli_options *options);
int execute_command(char *command);

#endif /* CLI_H */