#include "shell.h"

int main(int argc, char const *argv[]) {
    /* load config */
    shell_config();

    /* main running loop */
    shell_loop();

    /* shutdown and cleanup */
    shell_cleanup();

    return 0;
}

/*
 * read config file and load environment variables 
 */
void shell_config(){
    char *line;
    char **args;
    FILE *file;

    clear();
    printf("UNIX SHELL, version 1.0.0\n");
    printf("Type 'help' to see this list.\n\n");

    // For debugging pourposes
    //printf("PATH : %s\n", getenv("PATH"));
    //printf("HOME : %s\n", getenv("HOME"));
    //printf("ROOT : %s\n\n", getenv("ROOT"));

    printf("READING CONFIG...\n");
    file = fopen(".shellrc", "ab+");
    if(file){
        line = read_line(file);

        while(line[0] != '\n' && line[0] != '\0'){
            printf("\t%s\n", line);
            args = split_line(line);
            if(!execute(args)){
                printf("\tUnable to run command: %s!\n", line);
            }
            free(args);
            free(line);
            line = read_line(file);
        }
        free(line);
    }
    fclose(file);
    // Initialize history stack
    history = NULL;

    printf("\n");
}
 
/*
 * main running loop of shell 
 */
void shell_loop(){
    char *line;
    char **args;
    int status;

    do{
        printf("? ");
        line = read_line(stdin);
        save_history(line);
        args = split_line(line);
        status = execute(args);

        free(line);
        free(args);
    }while(status);
}

/*
 * Frees history stack
 */
void shell_cleanup(){
    // Clears up history stack
    if(history != NULL){
        while(history->next != NULL){
            struct node* temp = history->next;
            free(history->data);
            free(history);
            history = temp;
        }
        free(history);
    }
}


//
char* read_line(FILE* stream){
    char* buffer = malloc(sizeof(char) * BUFFER_SIZE);

    // Checks buffer has been allocated
    if(!buffer){
        fprintf(stderr, "? Warning: Allocation error!\n");
        exit(EXIT_FAILURE);
    }

    char c;
    int buf_size = BUFFER_SIZE;
    int position = 0;

    for(;;){
        c = getc(stream);
        if(c == '\n'){
            buffer[position] = '\0';
            return buffer;
        }
        else if(c == EOF){
            buffer[position] = '\n';
            return buffer;
        }
        else {
            buffer[position] = c;
        }
        position++;

        if(position >= buf_size){
            buffer = realloc(buffer, buf_size);
            if(!buffer){
                fprintf(stderr, "? Warning: Allocation error!\n");
                exit(EXIT_FAILURE);
            }
            buf_size += BUFFER_SIZE;
        }
    }
}

//
char** split_line(char* line){
    char** tokens = malloc(sizeof(char*) * TOKEN_SIZE);
    if(!tokens){
        fprintf(stderr, "? Warning: Allocation error!\n");
        exit(EXIT_FAILURE);
    }

    char* token = strtok(line, " ");
    int buf_size = TOKEN_SIZE;
    int position = 0;

    while(token){
        tokens[position] = token;

        position++;
        if(position >= buf_size){
            tokens = realloc(tokens, buf_size);
            if(!tokens){
                fprintf(stderr, "? Warning: Allocation error!\n");
                exit(EXIT_FAILURE);
            }
            buf_size += TOKEN_SIZE;
        }

        token = strtok(NULL, " ");
    }

    tokens[position] = NULL;
    return tokens;
}

// If built in command runs respective function, otherwise runs run_process
int execute(char** args){
    if(args[0] == NULL){
        return 1;
    }
    // Handles ctrl D exit
    if(args[0][0] == '\n'){
        return 0;
    }

    // Runs built-in shell functions
    int i;
    for(i = 0; i < NUM_BUILTIN; i++){
        if (strcmp(*args, builtin_str[i]) == 0){
            return (*builtin_func[i])(args);
        }
    }
    // Starts process
    return run_process(args);
}

// Runs fork
int run_process(char** args){
    pid_t pid;
    int status;

    pid = fork();
    // Fork sys call failed
    if(pid < 0){
        fprintf(stderr, "? Fork system call failed!\n");
        return 1;
    }
    // Child process
    else if(pid == 0){
        signal(SIGINT, SIG_DFL);
        if (execvp(*args, args) < 0) {
            fprintf(stderr, "? Program not found: %s\n", *args);
        }
    }
    // Parent process
    else{
        signal(SIGINT, SIG_IGN);
        while (wait(&status) != pid);
    }
    return 1;
}

// Adds value to history stack
int save_history(char* line){
    struct node* nnode = (struct node*)malloc(sizeof(struct node));
    if(!nnode){
        fprintf(stderr, "? Allocation error!\n");
        return 1;
    }
    char * copy = malloc(strlen(line) + 1);
    if(!copy){
        fprintf(stderr, "? Allocation error!\n");
        return 1;
    }
    strcpy(copy, line);
    nnode->data = copy;
    nnode->next = NULL;
    if(history == NULL){
        history = nnode;
    }else{
        struct node* temp = history;
        while(temp->next != NULL) temp = temp->next;
        temp->next = nnode;
    }
    return 1;
}

// Prints list of commands and discription to stdout
int help(char** args){
    int i;
    printf("\nBuilt in commands: \n");
    for(i = 0; i < NUM_BUILTIN; i++){
        printf("\t%s", builtin_str[i]);
        printf("%s\n", builtin_disc[i]);
    }
    printf("\n");
    return 1;
}

// Prints commands used to stdout
int print_history(char** args){
    struct node* temp = (struct node*)malloc(sizeof(struct node));
    temp = history;
    while(temp->next != NULL){
        printf(">%s\n", temp->data);
        temp = temp->next;
    }
    return 1;
}

// Changes the working directory
int change_dir(char** args){
    if(args[1] != NULL){
        if(chdir(args[1]) < 0){
            printf("? Could not open: %s\n", args[1]);
        }
        return 1;
    }
    else{
        if(chdir(getenv("HOME")) < 0){
            printf("? Could not open: %s\n", args[1]);
        }
        return 1;
    }
}

// Sets environment variables
int set_env(char** args){
    if(args[1] == NULL){
        printf("? Must specify a variable name!");
        return 1;
    }
    if(setenv(args[1], args[2], 0) < 0){
        printf("? Unable to set %s!\n", args[1]);
    }
    return 1;
}

// Unsets environment variables
int unset_env(char** args){
    if(args[1] == NULL){
        printf("? Must specify a variable name!");
    }
    if(unsetenv(args[1]) < 0){
        printf("Unable to unset %s!\n", args[1]);
    }
    return 1;
}

// Prints list of environment variables
int print_env(char** args){
    int i = 0;
    while(environ[i]) {
        printf("\t%s\n", environ[i++]); 
    }
    return 1;
}

// Returns status code to exit shell
int exit_shell(char** args){
    return 0;
}

