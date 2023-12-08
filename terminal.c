#include <termios.h>
#include <unistd.h>
// atexit() comes from <stdlib.h>.
#include <stdlib.h>
// sscanf() comes from <stdio.h>.
#include <stdio.h>
// ioctl(), TIOCGWINSZ, and struct winsize come from <sys/ioctl.h>.
#include <sys/ioctl.h>
#include <string.h>


#include "error.h"
#include "escape.h"
#include "editor.h"
#include "key.h"
#include "buffer.h"

extern struct termios orig_termios;
extern struct editorConfig E;

/**
 * @brief Get the Window Size object
 * 
 * @param rows 
 * @param cols 
 * @return int, error code 
 */
int getWindowSize(int *rows, int *cols)
{
  struct winsize ws;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    if (write(STDOUT_FILENO, ES_POSITION_CURSOR_RIGHT_BOTTOM, 
            ES_POSITION_CURSOR_RIGHT_BOTTOM_SIZE) != ES_POSITION_CURSOR_RIGHT_BOTTOM_SIZE) return -1;
    editorReadKey();
    return getCursorPosition(rows, cols);
  }
  else
  {
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
  }
}

/**
 * @brief Get the Cursor Position object
 * 
 * @param rows 
 * @param cols 
 * @return int 
 */
int getCursorPosition(int *rows, int *cols) {
  if (write(STDOUT_FILENO, ES_QUERY_CURSOR_POSITION, 
                          ES_QUERY_CURSOR_POSITION_SIZE) != ES_QUERY_CURSOR_POSITION_SIZE) return -1;
  char buf[32]; // \x1brow;colR
  unsigned int i = 0;
  
  while (i < sizeof(buf) - 1) {
    if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
    if (buf[i] == 'R') break;
    i++;
  }
  buf[i] = '\0';

  if (buf[0] != '\x1b' || buf[1] != '[') return -1;
  if (sscanf(&buf[2], "%d;%d", rows, cols) != 2) return -1;
  return 0;
}


/**
 * @brief disableRawMode
 * 
 */
void disableRawMode()
{
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
    die("tcsetattr");
}

/**
 * @brief enableRawMode
 * 
 */
void enableRawMode()
{
  if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1)
    die("tcgetattr");
  atexit(disableRawMode);

  struct termios raw = E.orig_termios;
  // tern off original setting
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

  // read timeout: size char and  time is 1/10 of a second
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    die("tcsetattr");
}



