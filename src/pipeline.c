#include "zensh.h"

int count_pipes(char **argv) {
  int count = 0;
  for (int i = 0; argv[i]; i++) {
    if (strcmp(argv[i], "|") == 0)
      count++;
  }
  return count;
}

int is_builtin(const char *cmd) {
  return strcmp(cmd, "echo") == 0 || strcmp(cmd, "pwd") == 0 ||
         strcmp(cmd, "type") == 0 || strcmp(cmd, "clear") == 0;
}

void run_builtin(char **argv) {
  if (strcmp(argv[0], "echo") == 0) {
    for (int i = 1; argv[i]; i++) {
      if (i > 1)
        putchar(' ');
      fputs(argv[i], stdout);
    }
    putchar('\n');
  } else if (strcmp(argv[0], "pwd") == 0) {
    pwd(NULL);
  } else if (strcmp(argv[0], "type") == 0) {
    for (int i = 1; argv[i]; i++) {
      type(argv[i]);
    }
  } else if (strcmp(argv[0], "clear") == 0) {
    clear_screen();
  }
}

int exec_pipeline(char **argv) {
  int num_pipes = count_pipes(argv);
  int num_cmds = num_pipes + 1;

  int pipefds[2 * num_pipes];

  for (int i = 0; i < num_pipes; i++) {
    if (pipe(pipefds + i * 2) < 0) {
      perror("pipe");
      return -1;
    }
  }

  int arg_start = 0;

  for (int i = 0; i < num_cmds; i++) {
    char *cmd_argv[MAX_ARGS];
    int k = 0;

    while (argv[arg_start] && strcmp(argv[arg_start], "|") != 0) {
      cmd_argv[k++] = argv[arg_start++];
    }
    cmd_argv[k] = NULL;

    if (argv[arg_start] && strcmp(argv[arg_start], "|") == 0) {
      arg_start++; // skip '|'
    }

    // 🚫 forbid cd in pipelines
    if (cmd_argv[0] && strcmp(cmd_argv[0], "cd") == 0) {
      fprintf(stderr, "cd: not allowed in pipeline\n");
      return -1;
    }

    pid_t pid = fork();

    if (pid == 0) {
      // stdin
      if (i > 0) {
        dup2(pipefds[(i - 1) * 2], STDIN_FILENO);
      }

      // stdout
      if (i < num_cmds - 1) {
        dup2(pipefds[i * 2 + 1], STDOUT_FILENO);
      }

      for (int j = 0; j < 2 * num_pipes; j++) {
        close(pipefds[j]);
      }

      if (redir(cmd_argv) < 0) {
        exit(1);
      }

      // ⭐ BUILTIN HANDLING HERE ⭐
      if (is_builtin(cmd_argv[0])) {
        run_builtin(cmd_argv);
        exit(0);
      }

      execvp(cmd_argv[0], cmd_argv);
      perror(cmd_argv[0]);
      exit(127);
    }
  }

  for (int i = 0; i < 2 * num_pipes; i++) {
    close(pipefds[i]);
  }

  for (int i = 0; i < num_cmds; i++) {
    wait(NULL);
  }

  return 0;
}
