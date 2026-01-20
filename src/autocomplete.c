#include "zensh.h"
#include <dirent.h>
#include <limits.h>

static const char *builtins[] = {"echo", "exit",  "type", "pwd",
                                 "cd",   "clear", NULL};

char *command_generator(const char *text, int state) {
  static int builtin_index;
  static DIR *dir;
  static char *path;
  static char *path_env;
  static char path_copy[4096];
  static char *saveptr;

  if (state == 0) {
    builtin_index = 0;

    if (dir) {
      closedir(dir);
      dir = NULL;
    }

    path_env = getenv("PATH");
    saveptr = NULL;

    if (path_env) {
      strncpy(path_copy, path_env, sizeof(path_copy) - 1);
      path_copy[sizeof(path_copy) - 1] = '\0';
      path = strtok_r(path_copy, ":", &saveptr);
    } else {
      path = NULL;
    }
  }

  while (builtins[builtin_index]) {
    const char *name = builtins[builtin_index++];
    if (strncmp(name, text, strlen(text)) == 0) {
      return strdup(name);
    }
  }

  /* while (path) {
    if (!dir) {
      dir = opendir(path);
      if (!dir) {
        path = strtok_r(NULL, ":", &saveptr);
        continue;
      }
    }

    struct dirent *entry;
    while ((entry = readdir(dir))) {
      if (strncmp(entry->d_name, text, strlen(text)) != 0) {
        continue;
      }

      char fullpath[PATH_MAX];
      snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

      if (access(fullpath, X_OK) == 0) {
        return strdup(entry->d_name);
      }
    }

    closedir(dir);
    dir = NULL;
    path = strtok_r(NULL, ":", &saveptr);
  } */

  return NULL;
}

char **zensh_completion(const char *text, int start, int end) {
  (void)end;

  if (start == 0) {
    return rl_completion_matches(text, command_generator);
  }

  return rl_completion_matches(text, rl_filename_completion_function);
}
