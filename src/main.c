#include <asm-generic/errno-base.h>
#include <errno.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_BUILTINS 3

void clear_screen() { printf("\033[2J\033[H"); }

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
  if (path_env == NULL) {
    printf("%s: not found\n", string);
    return;
  }

  char path_copy[4096];
  strncpy(path_copy, path_env, sizeof(path_copy) - 1);
  path_copy[sizeof(path_copy) - 1] = '\0';

  char *saveptr;
  char *dir = strtok_r(path_copy, ":", &saveptr);

  while (dir != NULL) {
    char full_path[PATH_MAX];

    snprintf(full_path, sizeof(full_path), "%s/%s", dir, string);

    if (access(full_path, X_OK) == 0) {
      printf("%s is %s\n", string, full_path);
      return;
    }

    dir = strtok_r(NULL, ":", &saveptr);
  }

  printf("%s: not found\n", string);
}

void exec_external(char *command, char *args) {
  char *argv[64];
  int argc = 0;

  argv[argc++] = command;

  if (args != NULL) {
    char *saveptr;
    char *arg = strtok_r(args, " ", &saveptr);

    while (arg != NULL && argc < 63) {
      argv[argc++] = arg;
      arg = strtok_r(NULL, " ", &saveptr);
    }
  }

  argv[argc] = NULL;

  pid_t pid = fork();

  if (pid == 0) {
    execvp(command, argv);
    // generally we just print perror(command) for posix style
    if (errno == ENOENT) {
      fprintf(stderr, "%s: command not found\n", command);
      exit(127);
    } else {
      perror(command);
      exit(1);
    }
  } else if (pid > 0) {
    waitpid(pid, NULL, 0);
  } else {
    perror("fork");
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
    if (strlen(input) == 0) {
      continue;
    }

    // exit command
    if (strcmp(input, "exit") == 0) {
      break;
    }

    if (strcmp(input, "clear") == 0) {
      clear_screen();
      continue;
    }

    // get the command
    char *saveptr;
    char *command = strtok_r(input, " ", &saveptr);
    if (command == NULL) {
      continue;
    }

    // echo command
    if (strcmp(command, "echo") == 0) {
      char *args = saveptr;
      echo(args);
    } else if (strcmp(command, "type") == 0) {
      char *arg;
      char *arg_saveptr;
      arg = strtok_r(saveptr, " ", &arg_saveptr);

      while (arg != NULL) {
        type(arg);
        arg = strtok_r(NULL, " ", &arg_saveptr);
      }
      continue;
    } else {
      exec_external(command, saveptr);
    }
  }

  return 0;
}
