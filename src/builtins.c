#include "zensh.h"
#include <ctype.h>

extern int history_session_start;

int history_session_start = 0;

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

int builtin_history(int argc, char **argv) {
  HIST_ENTRY **list = history_list();
  if (!list)
    return 0;

  int total = history_length;

  // history
  if (argc == 1) {
    int num = 1;
    for (int i = history_session_start; i < total; i++) {
      printf("%5d  %s\n", num++, list[i]->line);
    }
    return 0;
  }

  // history N
  if (argc == 2 && isdigit((unsigned char)argv[1][0])) {
    int n = atoi(argv[1]);
    int start = total - n;
    if (start < history_session_start)
      start = history_session_start;

    int num = 1;
    for (int i = start; i < total; i++) {
      printf("%5d  %s\n", num++, list[i]->line);
    }
    return 0;
  }

  // history -r FILE
  if (argc == 3 && strcmp(argv[1], "-r") == 0) {
    if (read_history(argv[2]) != 0) {
      perror("history");
      return 1;
    }
    return 0;
  }

  // history -w FILE
  if (argc == 3 && strcmp(argv[1], "-w") == 0) {
    if (write_history(argv[2]) != 0) {
      perror("history");
      return 1;
    }
    return 0;
  }

  return 0;
}
char *get_history_path(void) {
  const char *home = getenv("HOME");
  if (!home)
    return NULL;

  static char path[PATH_MAX];
  snprintf(path, sizeof(path), "%s/.zensh_history", home);
  return path;
}
