#include <stdio.h>
#include <string.h>

char *getCommand(char *input) {
  return strtok(input, " ");
}

char *getCommandArg(char *input){
  return strtok(NULL, "\0");
}

void echo(char *string) {
  if (string != NULL && *string != '\0') {
    printf("%s\n", string);
  }
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
    if(strlen(input)==0){
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
    } else {
      printf("%s: command not found\n", input);
    }
  }

  return 0;
}
