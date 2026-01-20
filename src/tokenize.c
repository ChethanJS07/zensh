#include "zensh.h"

int tokenize(const char *input, char **argv, int arg_max) {
  int argc = 0;
  char token[1024];
  int tlen = 0;

  enum {
    STATE_NORMAL,
    STATE_IN_SINGLE_QUOTES,
    STATE_IN_DOUBLE_QUOTES
  } state = STATE_NORMAL;

  for (int i = 0; input[i] != '\0'; i++) {
    char c = input[i];
    if (state == STATE_NORMAL) {
      if (c == ' ') {
        if (tlen > 0) {
          token[tlen] = '\0';
          if (argc < arg_max - 1) {
            argv[argc++] = strdup(token);
          }
          tlen = 0;
        }
      } else if (c == '\'') {
        state = STATE_IN_SINGLE_QUOTES;
      } else if (c == '\"') {
        state = STATE_IN_DOUBLE_QUOTES;
      } else if (c == '\\') {
        if (input[i + 1] != '\0') {
          token[tlen++] = input[++i];
        }
      } else {
        token[tlen++] = c;
      }
    } else if (state == STATE_IN_SINGLE_QUOTES) {
      if (c == '\'') {
        state = STATE_NORMAL;
      } else {
        token[tlen++] = c;
      }
    } else if (state == STATE_IN_DOUBLE_QUOTES) {
      if (c == '\"') {
        state = STATE_NORMAL;
      } else if (c == '\\') {
        if (input[i + 1] == '\"' || input[i + 1] == '\\') {
          token[tlen++] = input[++i];
        } else {
          token[tlen++] = c;
        }
      } else {
        token[tlen++] = c;
      }
    }
  }

  if (tlen > 0) {
    token[tlen] = '\0';
    if (argc < arg_max - 1) {
      argv[argc++] = strdup(token);
    }
  }
  argv[argc] = NULL;
  return argc;
}
