#ifndef SHELL_H
#define SHELL_H
#include "VGA_Text.h"
#include "types.h"
typedef struct{
    const char *command;
    const char *descript;
    void (*comm_handler)(uint16_t argc,char *argv[10]);
}commands_t;

void shell_init();
void cli_input_handler(const char comm);
void comm_execute(char *buffer, int size);
void cmd_help(uint16_t argc, char *argv[10]);
void cmd_say(uint16_t argc, char *argv[10]);
void cmd_clear(uint16_t argc, char *argv[10]);


#endif
