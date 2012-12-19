#ifndef _SHELL_COMMANDS_H
#define _SHELL_COMMANDS_H
void foo_fnt(int argc, char **argv);
void ls_fnt(int argc, char **argv);
void atoxi_fnt(int argc, char **argv);
void mkdir_fnt(int argc, char** argv);
void write_file_cli(int argc, char** argv);

unsigned long tinysh_atoxi(char *s);
/*
extern tinysh_cmd_t myfoocmd={0,"foo","foo command","[args]",foo_fnt,0,0,0};
extern tinysh_cmd_t atoxi_cmd={0,"atoxi","demonstrate atoxi support","[args-to-convert]",atoxi_fnt,0,0,0};

extern tinysh_cmd_t ls_cmd={0, "ls", "list files", "[args]",ls_fnt,0,0,0}; 

extern tinysh_cmd_t mkdir_cmd={0, "mkdir", "make a directory","usage: mkdir {directory}", mkdir_fnt, 0,0,0}; 
extern tinysh_cmd_t wr_cmd={0, "wr", "open and write to a file", "usage: wr {filename}", write_file_cli, 0,0,0};
*/
#endif
