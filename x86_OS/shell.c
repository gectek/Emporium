#include "shell.h"
#include "VGA_Text.h"
#include "stdfun.h"

static char command_buf[128];
static int cmd_index;

static commands_t command_table[] = {
    {"help", "Shows available commands", &cmd_help},
    {"clear", "Clear the terminal", &cmd_clear},
    {"say", "print whatever is after a space", &cmd_say},
    {0,0,0}
};
static int cmd_count = 3;
void shell_init()
{
    command_buf[0] = 0;
    cmd_index = 0;
    print("\nGectOS>");
}
void cli_input_handler(const char comm)
{
    if(comm == '\n'){
        command_buf[cmd_index] = '\0';  //terminating the string
        print("\n");
        int comm_size = str_len(command_buf);
        comm_execute(command_buf, comm_size);
        shell_init();
    }
    else if (comm == '\b') {
        if(cmd_index>0){
            clear_char();
            cmd_index--;
        }

    }
    else {
        command_buf[cmd_index++] = comm;
        tty_putc(comm);
    }
}
void comm_execute(char *buffer, int size)
{
    int i=0;
    char *cmd;
    uint16_t argc=0;
    char *argv[10];
    bool in_word=0;
    while(buffer[i] != '\0')        //For now ,this logic will only work for one space 
    {
        if(in_word == 0)
        {
            argv[argc] = &buffer[i];
            in_word = 1;
        }
        else if(in_word == 1 && buffer[i] == ' ')
        {
            argv[++argc] = &buffer[i];
            buffer[i] = 0;
            in_word = 0;
        }
        i++;
    }
    i=0;

    if(cmd_index == 0){
        return;
    }
    while(command_table[i].command != 0){

        if(str_cmp(argv[0], command_table[i].command) == 0)
        {
            command_table[i].comm_handler(argc, argv);
            break;
        }
        i++;
    }
    if(command_table[i].command == 0)
    {
        print("Unknown Command..!");
    }
    
}

void cmd_help(uint16_t argc, char *argv[10])
{
    int i;
    print("Available Commands:- \n");
    for(i=0; i<cmd_count; i++)
    {
        print("%s\n", command_table[i].command);
    }

}
void cmd_clear(uint16_t argc, char *argv[10])
{
    terminal_clear();
    return;
}
void cmd_say(uint16_t argc, char *argv[10])
{
    print("%s",argv[1]);
}
