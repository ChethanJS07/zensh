#include "zensh.h"
#include <ctype.h>
#include <readline/history.h>
#include <stdio.h>

int cd(char *args) {
  char *path;
  char expanded_path[PATH_MAX];

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
    snprintf(expanded_path, sizeof(expanded_path), "%s/%s", home, args + 2);
    path = expanded_path;
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

void type(char *args) {
  const char *builtins[6] = {"echo", "exit", "type", "pwd", "cd", "history"};
  for (int i = 0; i < 6; i++) {
    if (strcmp(args, builtins[i]) == 0) {
      printf("%s is a shell builtin\n", args);
      return;
    }
  }

  char *path_env = getenv("PATH");
  if (path_env == NULL) {
    printf("%s: is not found\n", args);
    return;
  }

  char path_copy[4096];
  strncpy(path_copy, path_env, sizeof(path_copy) - 1);
  path_copy[sizeof(path_copy) - 1] = '\0';

  char *saveptr;
  char *dir = strtok_r(path_copy, ":", &saveptr);

  while (dir != NULL) {
    char full_path[PATH_MAX];
    snprintf(full_path, sizeof(full_path), "%s/%s", dir, args);
    if (access(full_path, X_OK) == 0) {
      printf("%s is %s\n", args, full_path);
      return;
    }
    dir = strtok_r(NULL, ":", &saveptr);
  }
  printf("%s: not found\n", args);
}

void history(int argc, char **argv) {
  HIST_ENTRY **list = history_list();
  if (!list)
    return;

  int total = history_length;
  int start = 0;

  if (argc == 2) {
    for (char *p = argv[1]; *p; p++) {
      if (!isdigit(*p)) {
        fprintf(stderr, "history: %s: numeric argument required\n");
        return;
      }
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
      fprintf(stderr, "history: %s: invalid number\n", argv[1]);
      return;
    }

    if (n < total) {
      start = total - n;
    }
  } else if (argc > 2) {
    fprintf(stderr, "history: too many arguments\n");
    return;
  }

  for (int i = start; i < total; i++) {
    printf("%5d  %s\n", i + 1, list[i]->line);
  }
}
