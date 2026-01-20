#include "zensh.h"

void exec(char **argv) {
  pid_t pid = fork();

  if (pid == 0) {
    if (redir(argv) < 0) {
      exit(1);
    }
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
