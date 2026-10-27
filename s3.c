#include "s3.h"

///Simple for now, but will be expanded in a following section
void construct_shell_prompt(char shell_prompt[])
{
    strcpy(shell_prompt, "[s3]$ ");
}

///Prints a shell prompt and reads input from the user
void read_command_line(char line[])
{
    char shell_prompt[MAX_PROMPT_LEN];
    construct_shell_prompt(shell_prompt);
    printf("%s", shell_prompt);

    ///See man page of fgets(...)
    if (fgets(line, MAX_LINE, stdin) == NULL)
    {
        perror("fgets failed");
        exit(1);
    }
    ///Remove newline (enter)
    line[strlen(line) - 1] = '\0';
}

void parse_command(char line[], char *args[], int *argsc)
{
    ///Implements simple tokenization (space delimited)
    ///Note: strtok puts '\0' (null) characters within the existing storage, 
    ///to split it into logical cstrings.
    ///There is no dynamic allocation.

    ///See the man page of strtok(...)
    char *token = strtok(line, " ");
    *argsc = 0;
    while (token != NULL && *argsc < MAX_ARGS - 1)
    {
        args[(*argsc)++] = token;
        token = strtok(NULL, " ");
    }
    
    args[*argsc] = NULL; ///args must be null terminated
}

///Launch related functions
void child(char *args[], int argsc)
{
    ///Implement this function:

    ///Use execvp to load the binary 
    ///of the command specified in args[ARG_PROGNAME].
    ///For reference, see the code in lecture 3.

    execvp(args[ARG_PROGNAME],args);
    perror(args[ARG_PROGNAME]);
    exit(1);

}

void launch_program(char *args[], int argsc)
{
    ///Implement this function:

    ///fork() a child process.
    ///In the child part of the code,
    ///call child(args, argv)
    ///For reference, see the code in lecture 2.

    ///Handle the 'exit' command;
    ///so that the shell, not the child process,
    ///exits.
    
    if (args[0] == NULL){
        return;
    }
    if (strcmp(args[0],"exit") == 0){
        exit(0);
    }


    int rc = fork();
    if (rc<0) {
        perror("fork failed");
        exit(1);
    } 
    else if (rc==0){
        child(args, argsc);
    }
    else{
        // main will reap
    }
}

// does the line contain < or > ?
int command_with_redirection(char line[])
{
    // return true if line contains '<' OR '>'
    return strchr(line, '<') != NULL || strchr(line, '>') != NULL;
}

// 2. Find which word is the operator, and which kind it is
int find_redirection(char *args[], int argsc, int *type)
{
    for (int i = 0; i < argsc; i++) {
        // TODO: if args[i] is "<"  → set *type = 1, return i
        if (strcmp(args[i], ">") == 0) {*type = 1; return i;}
        // TODO: if args[i] is ">"  → set *type = 2, return i
        if (strcmp(args[i], ">>") == 0) {*type = 2; return i;}
        // TODO: if args[i] is ">>" → set *type = 3, return i
        if (strcmp(args[i], "<") == 0) {*type = 3; return i;}
    }
    *type = REDIR_NONE;
    return -1;   // no operator found
}
if (i <= 0 || i + 1 >= argsc) {          // no operator, nothing before it, or no filename after it
    fprintf(stderr, "s3: syntax error\n");
    return;
}

// 3. Parent side: save the filename, hide it, fork
void launch_program_with_redirection(char *args[], int argsc)
{
    int type;
    int i = find_redirection(args, argsc, &type);

    // char *filename = the word AFTER the operator
    char *filename = args[i+1];
    // put NULL where the operator is
    args[i] = NULL

    int rc = fork();
    if (rc == 0) {
        child_with_redirection(args, argsc, type, filename);
    }
}

// 4. Child side: point slot 0 or 1 at the file, then run the command
void child_with_redirection(char *args[], int argsc, int type, char *filename)
{
    int fd;
    // if type is 1 → fd = open(filename, O_RDONLY);
    if (type == 1){
        fd = open(filename, O_RDONLY);
    }
    // TODO: if type is 2 → fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    else if (type == 2){
        fd = open(filename, O_WRONLY, | O_CREAT | O_TRUNC, 0644);
    }
    // TODO: if type is 3 → fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    else (type ==3){
        fd = open(filename, O_WRONLY, | O_CREAT | O_APPEND, 0644);
    }

    if (fd < 0) {
        perror(filename);
        exit(1); 
    }

    if (type == 1) {
        dup2(fd, 0);
    } else {
        dup2(fd, 1);
    }
    
    close(fd);
    child(args, argsc);   
}
