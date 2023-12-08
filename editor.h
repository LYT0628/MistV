#ifndef __EDITOR_H__
#define __EDITOR_H__

#include <termios.h>
#include <unistd.h>
// atexit() comes from <stdlib.h>.
#include <stdlib.h>
// ioctl(), TIOCGWINSZ, and struct winsize come from <sys/ioctl.h>.
#include <sys/ioctl.h>


#include "error.h"
#include "escape.h"
#include "buffer.h"

#define LOVE_VERSION "0.0.1"
#define KILO_TAB_STOP 8


typedef struct erow {
  int size;
  int rsize;
  char *chars;
  char *render;
} erow;

struct editorConfig {
  int cx, cy;
  int rx;
  int screenrows;
  int screencols;

  int numrows;
  erow *row;
  int rowoff;
  int coloff;
  struct termios orig_termios;
};

void initEditor();
void editorOpen(char *filename);
void editorRefreshScreen();
void initEditor();
void editorScroll();
void editorAppendRow(char *s, size_t len);
void editorDrawRows(struct abuf *ab);
#endif

