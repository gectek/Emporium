#ifndef SHELL_H
#define SHELL_H
#include "VGA_Text.h"
#include "types.h"
typedef struct{
    const char *command;
    const char *descript;
    void (*comm_handler)(uint32_t argc,char *argv[10]);
}commands_t;

void shell_init();
void cli_input_handler(const char comm);
void comm_execute(char *buffer, int size);
void cmd_help(uint32_t argc, char *argv[10]);
void cmd_say(uint32_t argc, char *argv[10]);
void cmd_clear(uint32_t argc, char *argv[10]);
void frame_alloc(uint32_t argc, char *argv[10]);
void frame_free(uint32_t argc, char *argv[10]);
void k_panic(uint32_t argc, char *argv[10]);

#endif
