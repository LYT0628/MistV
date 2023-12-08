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

#define LOVE_VERSION "0.0.1"


struct editorConfig {
  int cx, cy;
  int screenrows;
  int screencols;
  struct termios orig_termios;
};
void initEditor();

#endif

