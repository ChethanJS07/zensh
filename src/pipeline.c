#include "zensh.h"

int count_pipes(char **argv) {
  int count = 0;
  for (int i = 0; argv[i]; i++) {
    if (strcmp(argv[i], "|") == 0)
      count++;
  }
  return count;
}

int exec_pipeline(char **argv) {
  int num_pipes = count_pipes(argv);
  int num_cmds = num_pipes + 1;

  int pipefds[2 * num_pipes];

  /* Create all pipes upfront */
  for (int i = 0; i < num_pipes; i++) {
    if (pipe(pipefds + i * 2) < 0) {
      perror("pipe");
      return -1;
    }
  }

  int cmd_index = 0;
  int arg_start = 0;

  for (int i = 0; i < num_cmds; i++) {
    /* Build argv for this command */
    char *cmd_argv[MAX_ARGS];
    int k = 0;

    while (argv[arg_start] && strcmp(argv[arg_start], "|") != 0) {
      cmd_argv[k++] = argv[arg_start++];
    }
    cmd_argv[k] = NULL;
    arg_start++; // skip '|'

    pid_t pid = fork();

    if (pid == 0) {
      /* stdin */
      if (i > 0) {
        dup2(pipefds[(i - 1) * 2], STDIN_FILENO);
      }

      /* stdout */
      if (i < num_cmds - 1) {
        dup2(pipefds[i * 2 + 1], STDOUT_FILENO);
      }

      /* Close all pipe fds in child */
      for (int j = 0; j < 2 * num_pipes; j++) {
        close(pipefds[j]);
      }

      /* Handle redirection inside pipeline */
      if (redir(cmd_argv) < 0) {
        exit(1);
      }

      execvp(cmd_argv[0], cmd_argv);
      perror(cmd_argv[0]);
      exit(127);
    }
  }

  /* Parent closes all pipes */
  for (int i = 0; i < 2 * num_pipes; i++) {
    close(pipefds[i]);
  }

  /* Wait for all children */
  for (int i = 0; i < num_cmds; i++) {
    wait(NULL);
  }

  return 0;
}
