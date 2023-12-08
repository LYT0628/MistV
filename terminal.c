#include <termios.h>
#include <unistd.h>
// atexit() comes from <stdlib.h>.
#include <stdlib.h>
// sscanf() comes from <stdio.h>.
#include <stdio.h>
// ioctl(), TIOCGWINSZ, and struct winsize come from <sys/ioctl.h>.
#include <sys/ioctl.h>

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
 * @brief draw tides
 * 
 */
void editorDrawRows(struct abuf *ab){
  int y;
  for (y = 0; y < E.screenrows; y++) {
    // draw welcome
    if (y == E.screenrows / 3) { 
      char welcome[80];
      int welcomelen = snprintf(welcome, sizeof(welcome),
        "Love editor -- version %s", LOVE_VERSION);
      if (welcomelen > E.screencols) welcomelen = E.screencols;
      abAppend(ab, welcome, welcomelen);
    } else {
      abAppend(ab, "~", 1);
    }

    abAppend(ab, ES_CLEAR_LINE, ES_CLEAR_LINE_SIZE);
    if (y < E.screenrows - 1) {
      abAppend(ab, "\r\n", 2);
    }
  }
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

// refresh screen
void editorRefreshScreen()
{
  struct abuf ab = ABUF_INIT;

  abAppend(&ab, ES_HIDE_CURSOR, ES_HIDE_CURSOR_SIZE);
  // abAppend(&ab, ES_CLEAR_ENTIRE_SCREEN, ES_CLEAR_ENTIRE_SCREEN_SIZE);
  abAppend(&ab,ES_POSITION_CURSOR_ORIGIN, ES_POSITION_CURSOR_ORIGIN_SIZE);

  editorDrawRows(&ab);
  
  abAppend(&ab, ES_POSITION_CURSOR_ORIGIN, ES_POSITION_CURSOR_ORIGIN_SIZE);
  abAppend(&ab, ES_SHOW_CURSOR, ES_SHOW_CURSOR_SIZE);

  write(STDOUT_FILENO, ab.b, ab.len);
  
  abFree(&ab);
}