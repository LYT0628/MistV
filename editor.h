#ifndef __EDITOR_H__
#define __EDITOR_H__

#include <termios.h>
#include <unistd.h>
// atexit() comes from <stdlib.h>.
#include <stdlib.h>
// ioctl(), TIOCGWINSZ, and struct winsize come from <sys/ioctl.h>.
#include <sys/ioctl.h>
#include <time.h>

#include "error.h"
#include "escape.h"
#include "buffer.h"

#define LOVE_VERSION "0.0.1"
#define KILO_TAB_STOP 8
#define KILO_QUIT_TIMES 3
#define HL_HIGHLIGHT_NUMBERS (1<<0)

struct editorSyntax {
  char *filetype;
  char **filematch;
  int flags;
};






typedef struct erow {
  int size;
  int rsize;
  char *chars;
  char *render;
  unsigned char *hl;
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
  int dirty;

  char *filename;

  char statusmsg[80];
  time_t statusmsg_time;

  struct editorSyntax *syntax;
  struct termios orig_termios;
};

void initEditor();
void editorOpen(char *filename);
void editorRefreshScreen();
void initEditor();
void editorScroll();
void editorDrawRows(struct abuf *ab);
void editorInsertChar(int c);
void editorSetStatusMessage(const char *fmt, ...);
char *editorPrompt(char *prompt, void (*callback)(char *, int));
#endif

