#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUM_BUILTINS 3

char *getCommand(char *input) { return strtok(input, " "); }

char *getCommandArg(char *input) { return strtok(NULL, "\0"); }

void echo(char *string) {
  if (string != NULL && *string != '\0') {
    printf("%s\n", string);
  }
}

void type(char *string) {
  const char *builtins[NUM_BUILTINS] = {"echo", "exit", "type"};

  for (int i = 0; i < NUM_BUILTINS; i++) {
    if (strcmp(string, builtins[i]) == 0) {
      printf("%s is a shell builtin\n", string);
      return;
    }
  }

  char *path_env = getenv("PATH");
  if(path_env==NULL){
    printf("%s: not found\n", string);
    return;
  }

  char path_copy[4096];
  strncpy(path_copy, path_env, sizeof(path_copy)-1);
  path_copy[sizeof(path_copy)-1]='\0';

  char *saveptr;
  char *dir = strtok_r(path_copy, ":", &saveptr);

  while(dir!=NULL){
    char full_path[PATH_MAX];

    snprintf(full_path, sizeof(full_path), "%s/%s", dir, string);

    if(access(full_path, X_OK)==0){
      printf("%s is %s\n", string, full_path);
      return;
    }

    dir = strtok_r(NULL, ":", &saveptr);
  }

  printf("%s: not found\n", string);
}

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  while (1) {
    printf("$ ");
    char input[1024];
    fgets(input, sizeof(input), stdin);

    input[strcspn(input, "\n")] = '\0';

    // empty input
    if (strlen(input) == 0) {
      continue;
    }

    // exit command
    if (strcmp(input, "exit") == 0) {
      break;
    }

    // get the command
    char *command = getCommand(input);
    char *command_arg = getCommandArg(input);

    // echo command
    if (strcmp(command, "echo") == 0) {
      echo(command_arg);
    } else if (strcmp(command, "type") == 0) {
      char *saveptr;
      char *arg = strtok_r(command_arg, " ", &saveptr);
      while (arg != NULL) {
        type(arg);
        arg = strtok_r(NULL, " ", &saveptr);
      }
    } else {
      printf("%s: command not found\n", input);
    }
  }

  return 0;
}
