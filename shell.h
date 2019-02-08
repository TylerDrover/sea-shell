#ifndef SHELL_H
#define SHELL_H

#include <signal.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define clear() printf("\033[H\033[J") 

#define BUFFER_SIZE 512
#define TOKEN_SIZE 32

#define NUM_BUILTIN 7

struct node {
  char* data;
  struct node* next;
};

// Stack for the history
struct node* history;

//Enviroment variables
extern char **environ;

void shell_config();
void shell_loop();
void shell_cleanup();

char* read_line(FILE* stream);
char** split_line(char* line);
int execute(char** args);
int run_process(char** args);
int save_history(char* args);

int help(char** args);
int print_history(char** args);
int change_dir(char** args);
int set_env(char** args);
int unset_env(char** args);
int print_env(char** args);
int exit_shell(char** args);

char *builtin_str[] = {
  "help",
  "history",
  "cd",
  "setenv",
  "unsetenv",
  "env",
  "exit"
};

char *builtin_disc[] = {
    " ",
    " \t\tprints out a list of the 20 previously used commands",
    " [dir] \t\tchanges working directory to dir or HOME if dir is omitted",
    " <var> [value] \tsets environment variable var to value if exists, otherwise create enironment variable var",
    " <var> \t\tdistroys an environment variable",
    " \t\t\tprints list of the current environment variables",
    " \t\t\tterminates current session"
};

int (*builtin_func[]) (char **) = {
  &help,
  &print_history,
  &change_dir,
  &set_env,
  &unset_env,
  &print_env,
  &exit_shell
};

#endif 