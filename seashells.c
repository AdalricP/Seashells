#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>

#define DELIM " \t\n"


/* Reading & Interpreting The Input*/
char *read_command(){
    char *line = NULL;
    size_t len = 0;
    ssize_t nchar; // number of characters

    printf("~2 ");
    
    nchar = getline(&line, &len, stdin);

    return line;
}

char **tokenize(char *line){
    char **tokens = NULL;
    char *token;
    size_t count=0;

    token = strtok(line, DELIM);

    // strtok only does its thing once till the DELIM
    // So I'll just run it through multiple times
    while (token != NULL) {
        tokens = realloc(tokens, (count + 1) * sizeof(char *)); // adds one extra slot.

        tokens[count] = strdup(token); // strdup duplicates a strng to a different pointer. Basically, we'll duplicate this token.

        count++;
        token = strtok(NULL, DELIM);
    }

    tokens = realloc(tokens, (count + 1) * sizeof(char *)); // resizes array to have one more slot

    tokens[count] = NULL; // makes that extra slot a null, so the array is null terminated

    return tokens;
}



/* PROCESSES */


int shell_launch(char **tokens){
    pid_t pid, wpid;
    int status;
    // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAHHHHHHHHHHHHHHHHHH
    // why must you inflict this suffering upon me, o lord.
    // O almighty father in heaven, what have i done to deserve this.
    // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAHHHHHHHHHHHHHHHHHH
    pid = fork(); 
    if (pid == 0) {
      if (execvp(tokens[0], tokens) == -1) {
        perror("~1 extex");
      }
      exit(EXIT_FAILURE);
    } else if (pid < 0) {
      perror("~1 extex");
    } else {
      do {
        wpid = waitpid(pid, &status, WUNTRACED);
      } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
  
    return 1;
}

/* BUILTINS */

int builtin_cd(char **tokens);
int builtin_help(char **tokens);
int builtin_exit(char **tokens);
// int builtin_color(char **tokens); // TODO
int builtin_seashell(char **tokens);
int builtin_clear(char **tokens);
int builtin_extex(char **tokens);


char *builtin_str[] = {
    "cd",
    "help",
    "exit",
    // "color",
    "seashell",
    "clear",
    "extex"
  };

typedef struct {
    char *command;
    char *description;
} Command;

Command builtin_def[] = {
    {"cd", "cd [directory]: lists current directory if no directory entered, else moves to given directory."},
    {"help", "help [command]: lists all builtin commands. If a command is given, describes that command. (like this)."},
    {"exit", "exit: exits the seashell, and goes to the seashore."},
    {"seashell", "seashell: describes what this project is about..."},
    {"clear", "clear: clears the screen."},
    {"extex", "extex [ext_command]: (External Execution) This lets you run Linux Commands within seashell."}
};

#define defined_commands (sizeof(builtin_def) / sizeof(builtin_def[0]))

int (*builtin_func[]) (char **) = {
  &builtin_cd,
  &builtin_help, //Forward Declaration because help showcases itself.
  &builtin_exit,
  // &builtin_color,
  &builtin_seashell,
  &builtin_clear,
  &builtin_extex
};

int num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

int builtin_cd(char **tokens)
{
  if (tokens[1] == NULL) {
    fprintf(stderr, "Expected Argument for \"cd\"\n");
  } else {
    if (chdir(tokens[1]) != 0) {
      perror("~!");
    }
  }
  return 1;
}

int builtin_help(char **tokens)
{
    if (tokens[1] == NULL) {
        int i;
        printf("Welcome To Aryan's Sea(Shell)!\n");
        printf("List Of BuiltIn Functions\n");

        for (i = 0; i < num_builtins(); i++) {
            printf("  %s\n", builtin_str[i]);   
        }

        printf("For information on other commands, use help {COMMAND}. :)\n");
    } else {
        int i;
        
        if (chdir(tokens[1]) != 0) {
            for (i = 0; i < defined_commands; i++) {
                if (strcmp(builtin_def[i].command, tokens[1]) == 0) {
                    printf("  %s\n", builtin_def[i].description);   
                }
            }
            printf("~1 Command not found.\n");
        }
    }
  return 1;
}

int builtin_exit(char **tokens)
{
  return 0;
}

int builtin_seashell(char **tokens){
    printf("Hey There! Welcome to\033[1;3;31m SeaShell by Aryan Pahwani.\033[0m\nThis is a micro-project I made within 2 hours to quickly teach myself a bit about how shells work.");
    printf("\nIf you want to view more of my projects,\nplease head over to\033[3;36m https://aryanpahwani.tech\033[0m :).\nI also write an ML blog @\033[3;36m https://www.neuristiq.com\033[0m if you're interested in machine learning.");
    printf("\nI'll add more to this project later, including piping, history, also more builtins & colors!");
    printf("\n\n");
    printf("\033[1;34;5m           _.-''|''-._\n");
    printf("        .-'     |     `-.\n");
    printf("      .'\\       |       /`.\n");
    printf("    .'   \\      |      /   `.\n");
    printf("    \\     \\     |     /     /\n");
    printf("     `\\    \\    |    /    /'\n");
    printf("       `\\   \\   |   /   /'\n");
    printf("         `\\  \\  |  /  /'\n");
    printf("        _.-`\\ \\ | / /'-._\n");
    printf("       {_____`\\\\|//'_____}\n");
    printf("               `-'\n\n\033[0m\n");
    return 1;
}

int builtin_clear(char **tokens){
    write(STDOUT_FILENO, "\033[H\033[2J", 7); // this looks hella gay
    system("tput reset"); // I know this creates an external dependancy, but look at who gives a shit, not me.
    return 1;
}

int builtin_extex(char **tokens){
    for (int i = 0; tokens[i] != NULL; i++) {
        tokens[i] = tokens[i + 1];
    }

    return shell_launch(tokens);
}

/* Mixes In Processes & Builtins */


int execute_command(char **tokens){
    int i;

    if (tokens[0] == NULL) {
      return 1;
    }
  
    for (i = 0; i < num_builtins(); i++) {
      if (strcmp(tokens[0], builtin_str[i]) == 0) {
        return (*builtin_func[i])(tokens);
      }
    }

    printf("~1 Command Not Found.\n"); // :(
    
    return 1;
}

/* Father, I desire the LööP 🫡 */
void shell_loop(){
    char *command;
    char **tokens; 

    int status = 1;

    builtin_clear(tokens);
    builtin_seashell(tokens);


    do {
        command = read_command(); // Reads & processes command
        tokens = tokenize(command);
        status = execute_command(tokens); // executtor? 🍆
        free(command);             
    } while (status);
}


int main(){

    printf("Starting Shell...\n");
    shell_loop();

    return 0;
}


