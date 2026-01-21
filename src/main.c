#include "zensh.h"

extern char **zensh_completion(const char *text, int start, int end);

static char *histfile;

static void save_history(void) {
  if (histfile) {
    write_history(histfile);
  }
}

int main(void) {
  setbuf(stdout, NULL);

  rl_attempted_completion_function = zensh_completion;

  histfile = get_history_path();
  if (histfile) {
    read_history(histfile);
  }

  history_session_start = 0;
  atexit(save_history);

  while (1) {
    char *line = readline("$ ");
    if (!line) {
      break;
    }

    if (*line) {
      add_history(line);
    }

    char *argv[MAX_ARGS];
    int argc = tokenize(line, argv, MAX_ARGS);
    free(line);

    if (argc == 0) {
      continue;
    }

    if (strcmp(argv[0], "exit") == 0) {
      break;
    }

    if (strcmp(argv[0], "clear") == 0) {
      clear_screen();
      goto cleanup;
    }

    if (strcmp(argv[0], "history") == 0) {
      builtin_history(argc, argv);
      goto cleanup;
    }

    if (strcmp(argv[0], "whobuiltyou") == 0) {
      whobuiltyou();
      goto cleanup;
    }

    int has_pipe = 0;
    for (int i = 0; argv[i]; i++) {
      if (strcmp(argv[i], "|") == 0) {
        has_pipe = 1;
        break;
      }
    }

    if (has_pipe) {
      exec_pipeline(argv);
      goto cleanup;
    }

    if (strcmp(argv[0], "echo") == 0) {
      int saved = dup(STDOUT_FILENO);
      if (redir(argv) == 0) {
        for (int i = 1; argv[i]; i++) {
          if (i > 1)
            putchar(' ');
          fputs(argv[i], stdout);
        }
        putchar('\n');
      }
      dup2(saved, STDOUT_FILENO);
      close(saved);
      goto cleanup;
    }

    if (strcmp(argv[0], "type") == 0) {
      for (int i = 1; argv[i]; i++) {
        type(argv[i]);
      }
      goto cleanup;
    }

    if (strcmp(argv[0], "pwd") == 0) {
      pwd(argv[1]);
      goto cleanup;
    }

    if (strcmp(argv[0], "cd") == 0) {
      cd(argv[1]);
      goto cleanup;
    }

    exec(argv);

  cleanup:
    for (int i = 0; argv[i]; i++) {
      free(argv[i]);
    }
  }

  return 0;
}
