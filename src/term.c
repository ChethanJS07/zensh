#include "zensh.h"
#include <termios.h>

static struct termios orig_termios;

void disable_raw_mode(void) {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enable_raw_mode(void) {
  struct termios raw;

  tcgetattr(STDIN_FILENO, &orig_termios);
  raw = orig_termios;

  raw.c_lflag &= ~(ICANON | ECHO); // no line buffering, no echo
  raw.c_cc[VMIN] = 1;
  raw.c_cc[VTIME] = 0;

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

char read_char() {
  char c;
  if (read(STDIN_FILENO, &c, 1) == 1)
    return c;
  return '\0';
}
