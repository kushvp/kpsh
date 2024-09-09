#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/* Global constants */
#define BUF_SIZE 1024
#define BUF_ARRAY_SIZE 64
#define ARGS_DELIMITER " \t\n\r\a"

/* Method Declaration */
void kpsh_loop(void);
char *kpsh_read_line();
char **kpsh_split_line(char *line);
void alloc_error_exit();
int kpsh_launch(char **args);
int kpsh_execute(char **args);
int kpsh_cd(char **args);
int kpsh_help();
int kpsh_exit();
int kpsh_num_builtins();

char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &kpsh_cd,
  &kpsh_help,
  &kpsh_exit
};

void kpsh_loop(void) {
  char *line;
  char **args;
  int status;

  do {
    printf("> ");
    line = kpsh_read_line();
    args = kpsh_split_line(line);
    status = kpsh_execute(args);

    free(line);
    free(args);
  } while(status);
}

char *kpsh_read_line() {
  int buffer_size = BUF_SIZE;
  int c;
  int position = 0;
  char *buffer = malloc(sizeof(char) * buffer_size);
  if(!buffer) {
    alloc_error_exit();
  }
  while(1) {
    c = getchar();
    if(c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    }
    buffer[position] = c;
    position++;

    if(position >= buffer_size) {
      buffer_size += BUF_SIZE;
      buffer = realloc(buffer, buffer_size);
      if(!buffer) {
        alloc_error_exit();
      }
    }
  }
}

char **kpsh_split_line(char *line) {
  int token_length = BUF_ARRAY_SIZE;
  char *token;
  int position = 0;

  char **tokens = malloc(sizeof(char*) * token_length);
  if(!tokens) {
    alloc_error_exit();
  }
  
  token = strtok(line, ARGS_DELIMITER);
  while(token != NULL) {
    tokens[position] = token;
    position++;

    if(position >= token_length) {
      token_length += BUF_ARRAY_SIZE;
      tokens = realloc(tokens, sizeof(char*) * token_length);
      if(!tokens) {
        alloc_error_exit();
      }
    }
    token = strtok(NULL, ARGS_DELIMITER);
  }
  tokens[position] = NULL;
  return tokens;
}

void alloc_error_exit() {
  fprintf(stderr, "kpsh: Allocation error\n");
  exit(EXIT_FAILURE);
}

int kpsh_launch(char **args) {
  pid_t pid;
  int status;

  pid = fork();
  if(pid == 0) {
    if(execvp(args[0], args) == -1) {
      perror("kpsh");
    }
  } else if(pid < 0) {
    perror("kpsh");
  } else {
    do {
      waitpid(pid, &status, WUNTRACED);
    } while(!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  return 1;
}

int kpsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

int kpsh_cd(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "kpsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("kpsh");
    }
  }
  return 1;
}

int kpsh_help() {
  int i;
  printf("Kush Patel's KPSH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < kpsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

int kpsh_exit() {
  return 0;
}

int kpsh_execute(char **args) {
  int i;

  if (args[0] == NULL) {
    return 1;
  }

  for (i = 0; i < kpsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return kpsh_launch(args);
}

/* Main method */
int main() {
  kpsh_loop();
  return EXIT_SUCCESS;
}
