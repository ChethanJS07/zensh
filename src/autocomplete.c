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

      if (len + remaining >= bufsize) {
        return 0;
      }
      strcpy(buffer + len, builtins[i] + prefix_len);
      fputs(builtins[i] + prefix_len, stdout);
      *cursor += remaining;
      return 1;
    }
  }

  return 0;
}
