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
#include "editor.h"
#include "key.h"

extern struct editorConfig E;

// read input
char editorReadKey() {
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN) die("read");
  }

  // arrow key tarts with '\x1b', '[', followed by an 'A', 'B', 'C', or 'D' 
  if (c == ES_PREFIX_0) {
    char seq[3];
    if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
    if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';
    if (seq[0] == ES_PREFIX_1) {
        // page up/down
        if (seq[1] >= '0' && seq[1] <= '9') {
          if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
          if (seq[2] == '~') {
            switch (seq[1]) {
              case '1': return HOME_KEY;
              case '3': return DEL_KEY;
              case '4': return END_KEY;
              case '5': return PAGE_UP;
              case '6': return PAGE_DOWN;
              case '7': return HOME_KEY;
              case '8': return END_KEY;
            }
          }
        } else { 
          //arrow key
          switch (seq[1]) {
            case 'A': return ARROW_UP;
            case 'B': return ARROW_DOWN;
            case 'C': return ARROW_RIGHT;
            case 'D': return ARROW_LEFT;
            case 'H': return HOME_KEY;
            case 'F': return END_KEY;
          }
        }
      }else if (seq[0] == 'O') {
      switch (seq[1]) {
        case 'H': return HOME_KEY;
        case 'F': return END_KEY;
      }
    }
    return '\x1b';
  } else {
    return c;
  }
  return c;
}

void editorMoveCursor(int key) {
  switch (key) {
    case ARROW_LEFT:
      if (E.cx != 0) {
        E.cx--;
      }
      break;
    case ARROW_RIGHT:
      if (E.cx != E.screencols - 1) {
        E.cx++;
      }
      break;
    case ARROW_UP:
      if (E.cy != 0) {
        E.cy--;
      }
      break;
    case ARROW_DOWN:
      if (E.cy != E.screenrows - 1) {
        E.cy++;
      }
      break;
  }
}

// read process key input
void editorProcessKeypress() {
  int c = editorReadKey();
  switch (c) {
    case CTRL_KEY('q'):
      write(STDOUT_FILENO, ES_CLEAR_ENTIRE_SCREEN, ES_CLEAR_ENTIRE_SCREEN_SIZE);
      write(STDOUT_FILENO, ES_POSITION_CURSOR_ORIGIN, ES_POSITION_CURSOR_ORIGIN_SIZE);
      exit(0);
      break;

    case HOME_KEY:
      E.cx = 0;
      break;
    case END_KEY:
      E.cx = E.screencols - 1;
      break;

    case PAGE_UP:
    case PAGE_DOWN:
      {
        int times = E.screenrows;
        while (times--)
          editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
      }
      break;
    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_LEFT:
    case ARROW_RIGHT:
      editorMoveCursor(c);
      break;
  }
}