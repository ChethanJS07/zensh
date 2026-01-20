#include "zensh.h"

int redir(char **argv) {
  for (int i = 0; argv[i] != NULL; i++) {
    int fd_target = -1;
    int append = 0;

    if (strcmp(argv[i], ">>") == 0) {
      fd_target = STDOUT_FILENO;
      append = 1;
    } else if (strcmp(argv[i], ">") == 0) {
      fd_target = STDOUT_FILENO;
      append = 0;
    } else if (argv[i][0] >= '0' && argv[i][0] <= '9' && argv[i][1] == '>' &&
               argv[i][2] == '>' && argv[i][3] == '\0') {
      fd_target = argv[i][0] - '0';
      append = 1;
    } else if (argv[i][0] >= '0' && argv[i][0] <= '9' && argv[i][1] == '>' &&
               argv[i][2] == '\0') {
      fd_target = argv[i][0] - '0';
      append = 0;
    }

    if (fd_target != -1) {
      if (argv[i + 1] == NULL) {
        fprintf(stderr, "syntax error: expected filename after '>'\n");
        return -1;
      }

      int flags = O_WRONLY | O_CREAT;
      if (append) {
        flags |= O_APPEND;
      } else {
        flags |= O_TRUNC;
      }

      int fd = open(argv[i + 1], flags, 0644);

      if (fd < 0) {
        perror(argv[i + 1]);
        return -1;
      }

      dup2(fd, fd_target);
      close(fd);

      int j;
      for (j = i; argv[j + 2] != NULL; j++) {
        argv[j] = argv[j + 2];
      }
      argv[j] = NULL;
      i--;
    }
  }
  return 0;
}
