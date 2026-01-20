#include "zensh.h"
#include <stdlib.h>

int main(void) {
  setbuf(stdout, NULL);

  enable_raw_mode();
  atexit(disable_raw_mode);

  while (1) {
    printf("$ ");

    char input[MAX_LINE];
    size_t len = 0;
    size_t cursor = 0;

    while (1) {
      char c = read_char();

      if (c == '\n' || c == '\r') {
        input[len] = '\0';
        putchar('\n');
        break;
      }

      if (c == 127 || c == '\b') { // backspace
        if (len > 0) {
          len--;
          cursor--;
          printf("\b \b");
        }
        continue;
      }

      if (c == '\t') {
        autocomplete(input, sizeof(input), &cursor);
        len = cursor;
        continue;
      }

      if (len < MAX_LINE - 1) {
        input[len++] = c;
        cursor++;
        putchar(c);
      }
    }
    if (*input == '\0') {
      continue;
    }

    char *argv[MAX_ARGS];
    int argc = tokenize(input, argv, MAX_ARGS);
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
