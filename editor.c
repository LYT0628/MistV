#include <termios.h>
#include "editor.h"
#include "error.h"
#include "terminal.h"

// original terminal setting
struct termios orig_termios;

// global editor configuration
struct editorConfig E;


void initEditor()
{
  E.cx = 0;
  E.cy = 0;
  
  if (getWindowSize(&E.screenrows, &E.screencols) == -1)
    die("getWindowSize");
}