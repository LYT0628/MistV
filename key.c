//read() and STDIN_FILENO come from <unistd.h>
#include <unistd.h>
// iscntrl() comes from <ctype.h>
#include <ctype.h>
// printf() comes from <stdio.h>
#include <stdio.h>
// EAGAIN comes from 
#include <errno.h>


#include "terminal.h"
#include "error.h"
#include "escape.h"

// read input
char editorReadKey() {
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN) die("read");
  }
  return c;
}

// read process key input
void editorProcessKeypress() {
  char c = editorReadKey();
  switch (c) {
    case CTRL_KEY('q'):
      write(STDOUT_FILENO, ES_CLEAR_ENTIRE_SCREEN, ES_CLEAR_ENTIRE_SCREEN_SIZE);
      write(STDOUT_FILENO, ES_POSITION_CURSOR_ORIGIN, ES_POSITION_CURSOR_ORIGIN_SIZE);
      exit(0);
      break;
  }
}