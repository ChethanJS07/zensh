#include "zensh.h"

static const char *builtins[] = {"echo", "exit",  "type", "pwd",
                                 "cd",   "clear", NULL};

int autocomplete(char *buffer, size_t bufsize, size_t *cursor) {
  size_t len = *cursor;

  size_t start = len;
  while (start > 0 && buffer[start - 1] != ' ') {
    start--;
  }
  size_t prefix_len = len - start;
  if (prefix_len == 0) {
    return 0;
  }

  for (int i = 0; builtins[i]; i++) {
    if (strncmp(builtins[i], buffer + start, prefix_len) == 0) {
      size_t remaining = strlen(builtins[i]) - prefix_len;

      if (len + remaining + 2 >= bufsize) {
        return 0;
      }
      strcpy(buffer + len, builtins[i] + prefix_len);
      fputs(builtins[i] + prefix_len, stdout);
      *cursor += remaining;
      if (*cursor == start + strlen(builtins[i])) {
        buffer[*cursor] = ' ';
        buffer[*cursor + 1] = '\0';
        putchar(' ');
        (*cursor)++;
      }
      return 1;
    }
  }

  return 0;
}
