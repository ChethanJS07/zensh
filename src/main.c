#include <errno.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void clear_screen() { printf("\033[2J\033[H"); }

int cd(char *args) {
  char *path;
  char expanded[PATH_MAX];

  if (args == NULL || strlen(args) == 0) {
    path = getenv("HOME");
    if (path == NULL) {
      fprintf(stderr, "cd: HOME not set\n");
      return 1;
    }
  } else if (strcmp(args, "~") == 0) {
    path = getenv("HOME");
    if (path == NULL) {
      fprintf(stderr, "cd: HOME not set\n");
      return 1;
    }
  } else if (args[0] == '~' && args[1] == '/') {
    char *home = getenv("HOME");
    if (home == NULL) {
      fprintf(stderr, "cd: HOME not set\n");
      return 1;
    }
    snprintf(expanded, sizeof(expanded), "%s/%s", home, args + 2);
    path = expanded;
  } else {
    path = args;
  }

  if (chdir(path) != 0) {
    fprintf(stderr, "cd: %s: %s\n", path, strerror(errno));
    return 1;
  }
  return 0;
}

int pwd(char *args) {
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    printf("%s\n", cwd);
  } else {
    perror("pwd");
    return 1;
  }
  return 0;
}

void type(char *string) {
  const char *builtins[5] = {"echo", "exit", "type", "pwd", "cd"};

  for (int i = 0; i < 5; i++) {
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

void exec_external(char **argv) {
  pid_t pid = fork();

  if (pid == 0) {
    execvp(argv[0], argv);
    if (errno == ENOENT) {
      fprintf(stderr, "%s: command not found\n", argv[0]);
      exit(127);
    } else {
      perror(argv[0]);
      exit(1);
    }
  } else if (pid > 0) {
    waitpid(pid, NULL, 0);
  } else {
    perror("fork");
  }
}

int tokenize(const char *input, char **argv, int arg_max) {
  int argc = 0;
  char token[1024];
  int tlen = 0;

  enum { STATE_NORMAL, STATE_IN_SINGLE_QUOTE } state = STATE_NORMAL;

  for (int i = 0; input[i] != '\0'; i++) {
    char c = input[i];
    if (state == STATE_NORMAL) {
      if (c == ' ') {
        if (tlen > 0) {
          token[tlen] = '\0';
          if (argc < arg_max - 1) {
            argv[argc++] = strdup(token);
          }
          tlen = 0;
        }
      } else if (c == '\'') {
        state = STATE_IN_SINGLE_QUOTE;
      } else {
        token[tlen++] = c;
      }
    } else if (state == STATE_IN_SINGLE_QUOTE) {
      if (c == '\'') {
        state = STATE_NORMAL;
      } else {
        token[tlen++] = c;
      }
    }
  }
  if (tlen > 0) {
    token[tlen] = '\0';
    if (argc < arg_max - 1) {
      argv[argc++] = strdup(token);
    }
  }
  argv[argc] = NULL;

  return argc;
}

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  while (1) {
    printf("🧘‍♂️ ");
    char input[1024];

    if (fgets(input, sizeof(input), stdin) == NULL) {
      break;
    }

    input[strcspn(input, "\n")] = '\0';

    // empty input
    if (strlen(input) == 0) {
      continue;
    }

    char *cmd_argv[64];
    int cmd_argc = tokenize(input, cmd_argv, 64);

    if (cmd_argc == 0) {
      continue;
    }

    // exit command
    if (strcmp(cmd_argv[0], "exit") == 0) {
      break;
    }

    // clear command
    if (strcmp(cmd_argv[0], "clear") == 0) {
      clear_screen();
      continue;
    }

    // echo command
    if (strcmp(cmd_argv[0], "echo") == 0) {
      for (int i = 1; i < cmd_argc; i++) {
        if (i > 1) {
          printf(" ");
        }
        printf("%s", cmd_argv[i]);
      }
      printf("\n");
    } else if (strcmp(cmd_argv[0], "type") == 0) {
      for (int i = 1; i < cmd_argc; i++) {
        type(cmd_argv[i]);
      }
    } else if (strcmp(cmd_argv[0], "pwd") == 0) {
      pwd(cmd_argv[1]);
    } else if (strcmp(cmd_argv[0], "cd") == 0) {
      cd(cmd_argv[1]);
    } else {
      exec_external(cmd_argv);
    }

    for (int i = 0; i < cmd_argc; i++) {
      free(cmd_argv[i]);
    }
  }

  return 0;
}
