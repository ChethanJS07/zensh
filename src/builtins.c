#include "zensh.h"
#include <linux/limits.h>

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
  const char *builtins[5] = {"echo", "exit", "type", "pwd", "cd"};
  for (int i = 0; i < 5; i++) {
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
