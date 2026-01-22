#include "zensh.h"

extern char **zensh_completion(const char *text, int start, int end);

static char *histfile;

static void save_history(void) {
  if (histfile) {
    write_history(histfile);
  }
}

int main(int argc, char **argv) {

  if (argc > 1) {
    if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0 ||
        strcmp(argv[1], "-V") == 0) {
      printf("zensh %s\n", ZENSH_VERSION);
      return 0;
    }
  }

  setbuf(stdout, NULL);

  rl_attempted_completion_function = zensh_completion;

  histfile = get_history_path();
  if (histfile) {
    read_history(histfile);
  }

  history_session_start = history_length;
  atexit(save_history);

  while (1) {
    char *line = readline("🧘‍♂️ ");
    if (!line) {
      break;
    }

    if (*line) {
      add_history(line);
    }

    char *argv_cmd[MAX_ARGS];
    int argc_cmd = tokenize(line, argv_cmd, MAX_ARGS);
    free(line);

    if (argc_cmd == 0) {
      continue;
    }

    if (strcmp(argv_cmd[0], "exit") == 0) {
      break;
    }

    if (strcmp(argv_cmd[0], "clear") == 0) {
      clear_screen();
      goto cleanup;
    }

    if (strcmp(argv_cmd[0], "history") == 0) {
      builtin_history(argc_cmd, argv_cmd);
      goto cleanup;
    }

    if (strcmp(argv_cmd[0], "whobuiltyou") == 0) {
      whobuiltyou();
      goto cleanup;
    }

    int has_pipe = 0;
    for (int i = 0; argv_cmd[i]; i++) {
      if (strcmp(argv_cmd[i], "|") == 0) {
        has_pipe = 1;
        break;
      }
    }

    if (has_pipe) {
      exec_pipeline(argv_cmd);
      goto cleanup;
    }

    if (strcmp(argv_cmd[0], "echo") == 0) {
      int saved = dup(STDOUT_FILENO);

      if (redir(argv_cmd) == 0) {
        for (int i = 1; argv_cmd[i]; i++) {
          if (i > 1)
            putchar(' ');
          fputs(argv_cmd[i], stdout);
        }
        putchar('\n');
      }

      dup2(saved, STDOUT_FILENO);
      close(saved);
      goto cleanup;
    }

    if (strcmp(argv_cmd[0], "type") == 0) {
      for (int i = 1; argv_cmd[i]; i++) {
        type(argv_cmd[i]);
      }
      goto cleanup;
    }

    if (strcmp(argv_cmd[0], "pwd") == 0) {
      pwd(argv_cmd[1]);
      goto cleanup;
    }

    if (strcmp(argv_cmd[0], "cd") == 0) {
      cd(argv_cmd[1]);
      goto cleanup;
    }

    exec(argv_cmd);

  cleanup:
    for (int i = 0; argv_cmd[i]; i++) {
      free(argv_cmd[i]);
    }
  }

  return 0;
}
