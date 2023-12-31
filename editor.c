#include <unistd.h>
#include <termios.h>
// ssize_t comes from <sys/types.h>.
#include <sys/types.h>
#include <stdlib.h>
// sscanf() comes from <stdio.h>.
// va_list, va_start(), and va_end() come from <stdarg.h>. vsnprintf() comes from <stdio.h>.
#include <stdio.h>
// ioctl(), TIOCGWINSZ, and struct winsize come from <sys/ioctl.h>.
// #include <sys/ioctl.h>
#include <string.h>
// time() comes from <time.h>.
#include <time.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>
#include <malloc.h>
#include <errno.h>
// strdup() comes from <string.h>
//  It makes a copy of the given string, 
//allocating the required memory and assuming you will free() that memory.
#include <string.h>
#include <fcntl.h>



#include "key.h"
#include "editor.h"
#include "error.h"
#include "buffer.h"
#include "terminal.h"
#include "highlight.h"

char* mystrdup(const char* s){
  size_t len = strlen(s)+1;
  void* new  = malloc(len);
  if(len == NULL){
    return NULL;
  }
  return (char*) memcpy(new ,s ,len);
}



// original terminal setting
struct termios orig_termios;

// global editor configuration
struct editorConfig E;

/*** filetypes ***/
char *C_HL_extensions[] = { ".c", ".h", ".cpp", NULL };
char *C_HL_keywords[] = {
  "switch", "if", "while", "for", "break", "continue", "return", "else",
  "struct", "union", "typedef", "static", "enum", "class", "case",
  "int|", "long|", "double|", "float|", "char|", "unsigned|", "signed|",
  "void|", NULL
};
struct editorSyntax HLDB[] = {
  {
    "c",
    C_HL_extensions,
    "//", "/*", "*/",// comment
    C_HL_keywords, // keywords
    HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
  },
};
#define HLDB_ENTRIES (sizeof(HLDB) / sizeof(HLDB[0]))

/**!SECTION
 * Row operation*
 */
int editorRowCxToRx(erow *row, int cx) {
  int rx = 0;
  int j;
  for (j = 0; j < cx; j++) {
    if (row->chars[j] == '\t')
      rx += (KILO_TAB_STOP - 1) - (rx % KILO_TAB_STOP);
    rx++;
  }
  return rx;
}

/**
 * @brief 
 * 
 * @param row 
 * @param rx 
 * @return int 
 */
int editorRowRxToCx(erow *row, int rx) {
  int cur_rx = 0;
  int cx;
  for (cx = 0; cx < row->size; cx++) {
    if (row->chars[cx] == '\t')
      cur_rx += (KILO_TAB_STOP - 1) - (cur_rx % KILO_TAB_STOP);
    cur_rx++;
    if (cur_rx > rx) return cx;
  }
  return cx;
}

void editorUpdateRow(erow *row) {
  int tabs = 0;
  int j;
  for (j = 0; j < row->size; j++)
    if (row->chars[j] == '\t') tabs++;

  free(row->render);
  row->render = malloc(row->size + tabs*(KILO_TAB_STOP - 1) + 1);//render 8 space
  int idx = 0;
  for (j = 0; j < row->size; j++) {
    if (row->chars[j] == '\t') {
      row->render[idx++] = ' ';
      while (idx % KILO_TAB_STOP != 0) row->render[idx++] = ' ';
    } else {
      row->render[idx++] = row->chars[j];
    }
  }
  row->render[idx] = '\0';
  row->rsize = idx;

  editorUpdateSyntax(row);
}

/**
 * @brief append a line to editorConfig with 
 * 
 * @param s point to the string of line
 * @param len size of the string of line
 */
void editorInsertRow(int at, char *s, size_t len) {
  if (at < 0 || at > E.numrows) return;
  E.row = realloc(E.row, sizeof(erow) * (E.numrows + 1));
  memmove(&E.row[at + 1], &E.row[at], sizeof(erow) * (E.numrows - at));
  
  // when inserting a new row, we need to update row index after it.
  for (int j = at + 1; j <= E.numrows; j++) E.row[j].idx++;
  E.row[at].idx = at;
  E.row[at].size = len; 
  E.row[at].chars = malloc(len + 1);
  memcpy(E.row[at].chars, s, len);// dest, src, len
  E.row[at].chars[len] = '\0';

  E.row[at].rsize = 0;
  E.row[at].render = NULL;
  E.row[at].hl = NULL;
  E.row[at].hl_open_comment = 0; // in open multi comment
  editorUpdateRow(&E.row[at]);

  E.numrows++;

}




void editorOpen(char *filename) {
  free(E.filename);
  // strdup() comes from <string.h>. It makes a copy of the given string, 
  //allocating the required memory and assuming you will free() that memory.
  E.filename = mystrdup(filename);


  editorSelectSyntaxHighlight();
  FILE *fp = fopen(filename, "r");
  if (!fp) die("fopen");

  // store buffer
  char *line = NULL;
  size_t linecap = 0;
  ssize_t linelen;

  // remains data to read
  while ((linelen = getline(&line, &linecap, fp)) != -1) {
    // blank line
    while (linelen > 0 && (line[linelen - 1] == '\n' ||
                      line[linelen - 1] == '\r'))
      linelen--;
      editorInsertRow(E.numrows, line, linelen); // newline

  }

  free(line);
  fclose(fp);
  E.dirty = 0;
}

   
/**
 * @brief 
 * 
 * @param buflen 
 * @return char* 
 */
char *editorRowsToString(int *buflen) {
  int totlen = 0;
  int j;
  for (j = 0; j < E.numrows; j++)
    totlen += E.row[j].size + 1;
  *buflen = totlen;
  char *buf = malloc(totlen);
  char *p = buf;
  for (j = 0; j < E.numrows; j++) {
    memcpy(p, E.row[j].chars, E.row[j].size);
    p += E.row[j].size;
    *p = '\n';
    p++;
  }
  return buf;
}

void editorSave() {
  if (E.filename == NULL) {
    E.filename = editorPrompt("Save as: %s (ESC to cancel)", NULL);
    if (E.filename == NULL) {
      editorSetStatusMessage("Save aborted");
      return;
    }
    editorSelectSyntaxHighlight();
  }


  if (E.filename == NULL) return;
  int len;
  char *buf = editorRowsToString(&len);
  int fd = open(E.filename, O_RDWR | O_CREAT, 0644);
  if (fd != -1) {
    if (ftruncate(fd, len) != -1) {
      if (write(fd, buf, len) == len) {
        close(fd);
        free(buf);
          E.dirty = 0;
        editorSetStatusMessage("%d bytes written to disk", len);
        return;
      }
    }
    close(fd);
  }
  free(buf);
  editorSetStatusMessage("Can't save! I/O error: %s", strerror(errno));
  
}

/**!SECTION 
 * draw  editor
 */

/***
 * scroll editor
 */
void editorScroll() {


  if (E.cy < E.numrows) {
    E.rx = editorRowCxToRx(&E.row[E.cy], E.cx);
  }


  if (E.cy < E.rowoff) {
    E.rowoff = E.cy;
  }
  if (E.cy >= E.rowoff + E.screenrows) {
    E.rowoff = E.cy - E.screenrows + 1;
  }
  if (E.rx < E.coloff) {
    E.coloff = E.rx;
  }
  if (E.rx >= E.coloff + E.screencols) {
    E.coloff = E.rx - E.screencols + 1;
  }
}



void editorDrawStatusBar(struct abuf *ab) {
  abAppend(ab, ES_TEXT_FORMAT_INVERTED_COLOR, ES_TEXT_FORMAT_INVERTED_COLOR_SIZE);

  char status[80], rstatus[80];

  int len = snprintf(status, sizeof(status), "%.20s - %d lines %s",
    E.filename ? E.filename : "[No Name]", E.numrows,
    E.dirty ? "(modified)" : "");

  int rlen = snprintf(rstatus, sizeof(rstatus), "%s | %d/%d",
    E.syntax ? E.syntax->filetype : "no ft", E.cy + 1, E.numrows); // current line / all lines， not filetype
 
  if (len > E.screencols) len = E.screencols;
  abAppend(ab, status, len);

  while (len < E.screencols) {
    
    if (E.screencols - len == rlen) { //right size render
      abAppend(ab, rstatus, rlen);
      break;
    } else {
      abAppend(ab, " ", 1);
      len++;
    }
  }
  abAppend(ab, ES_TEXT_FORMAT_RESET, ES_TEXT_FORMAT_RESET_SIZE);
  abAppend(ab, "\r\n", 2);
}

void editorDrawMessageBar(struct abuf *ab) {
  abAppend(ab, ES_CLEAR_LINE, ES_CLEAR_LINE_SIZE);
  int msglen = strlen(E.statusmsg);
  if (msglen > E.screencols) msglen = E.screencols;

  if (msglen && time(NULL) - E.statusmsg_time < 5) // only show 5 seconds
    abAppend(ab, E.statusmsg, msglen);
}

// refresh screen
void editorRefreshScreen()
{
  editorScroll(); // default scroll 0 , that is beginning
  // exit(0);
  struct abuf ab = ABUF_INIT;

  abAppend(&ab, ES_HIDE_CURSOR, ES_HIDE_CURSOR_SIZE);
  abAppend(&ab,ES_POSITION_CURSOR_ORIGIN, ES_POSITION_CURSOR_ORIGIN_SIZE);

  editorDrawRows(&ab);
  editorDrawStatusBar(&ab);
  editorDrawMessageBar(&ab);

  char buf[32];
  snprintf(buf, sizeof(buf), ES_POSITION_CURSOR_FORMAT, (E.cy - E.rowoff) + 1, 
                                                         (E.rx - E.coloff) + 1);

  abAppend(&ab, buf, strlen(buf));
  
  abAppend(&ab, ES_SHOW_CURSOR, ES_SHOW_CURSOR_SIZE);

  write(STDOUT_FILENO, ab.b, ab.len);
  
  abFree(&ab);
}

void editorSetStatusMessage(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(E.statusmsg, sizeof(E.statusmsg), fmt, ap);
  va_end(ap);
  E.statusmsg_time = time(NULL);
}

char *editorPrompt(char *prompt, void (*callback)(char *, int)) {
  size_t bufsize = 128;
  char *buf = malloc(bufsize);
  size_t buflen = 0;
  buf[0] = '\0';
  while (1) {
    editorSetStatusMessage(prompt, buf);
    editorRefreshScreen();
    int c = editorReadKey();
    if (c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE) {
      if (buflen != 0) buf[--buflen] = '\0';
    } else if (c == '\x1b') {
      editorSetStatusMessage("");
      if (callback) callback(buf, c);
      free(buf);
      return NULL;
    } else if (c == '\r') {
      if (buflen != 0) {
        editorSetStatusMessage("");
        if (callback) callback(buf, c);
        return buf;
      }
    } else if (!iscntrl(c) && c < 128) {
      if (buflen == bufsize - 1) {
        bufsize *= 2;
        buf = realloc(buf, bufsize);
      }
      buf[buflen++] = c;
      buf[buflen] = '\0';
    }
    if (callback) callback(buf, c);
  }
}

/**
 * @brief draw tides
 * 
 */
void editorDrawRows(struct abuf *ab){
  int y;

  for (y = 0; y < E.screenrows; y++) {
    // offset 
    int filerow = y + E.rowoff;
    if (filerow >= E.numrows) {
      // draw welcome
      if (E.numrows == 0 && y == E.screenrows / 3) {
        char welcome[80];
        int welcomelen = snprintf(welcome, sizeof(welcome),
          "Love editor -- version %s", LOVE_VERSION);
        if (welcomelen > E.screencols) welcomelen = E.screencols;
        // horizontal center
        int padding = (E.screencols - welcomelen) / 2;
        if (padding) {
          abAppend(ab, "~", 1);
          padding--;
        }
        while (padding--) abAppend(ab, " ", 1);

        abAppend(ab, welcome, welcomelen);
      } else {
        abAppend(ab, "~", 1);
      }
    } else {
      //draw file
      int len = E.row[filerow].rsize - E.coloff;
      if (len < 0) len = 0;
      if (len > E.screencols) len = E.screencols;
      
      char *c = &E.row[filerow].render[E.coloff];
      unsigned char *hl = &E.row[filerow].hl[E.coloff];
      int current_color = -1;
      int j;
      for (j = 0; j < len; j++) {
        // skip escape character
        if (iscntrl(c[j])) {
          char sym = (c[j] <= 26) ? '@' + c[j] : '?';
          abAppend(ab, "\x1b[7m", 4);
          abAppend(ab, &sym, 1);
          abAppend(ab, "\x1b[m", 3);
          // restore format setting
          if (current_color != -1) {
            char buf[16];
            int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", current_color);
            abAppend(ab, buf, clen);
          }

        } else if (hl[j] == HL_NORMAL) {
          if (current_color != -1) {
            abAppend(ab, ES_COLOR_RESET, ES_COLOR_RESET_SIZE);
            current_color = -1;
          }
          abAppend(ab, &c[j], 1);
        }else {
          int color = editorSyntaxToColor(hl[j]);
          if (color != current_color) { // keep coloring until color changed
            current_color = color;
            char buf[16];
            int clen = snprintf(buf, sizeof(buf), ES_COLOR_FORMAT, color);
            abAppend(ab, buf, clen);
          }
          abAppend(ab, &c[j], 1);
        }
      }
      abAppend(ab, ES_COLOR_RESET, ES_COLOR_RESET_SIZE);
    }


    abAppend(ab, ES_CLEAR_LINE, ES_CLEAR_LINE_SIZE);
    abAppend(ab, "\r\n", 2);
  }
}


/***
 * initial editor
 */
void initEditor()
{
  E.cx = 0;
  E.cy = 0;
  E.rx = 0;
  E.numrows = 0;
  E.row = NULL;
  E.rowoff = 0;
  E.coloff = 0;
  E.dirty = 0;
  E.filename = NULL;
  E.statusmsg[0] = '\0';
  E.statusmsg_time = 0;
  E.syntax = NULL;

  editorSetStatusMessage("HELP: Ctrl-Q = quit");

  if (getWindowSize(&E.screenrows, &E.screencols) == -1)
    die("getWindowSize");
  E.screenrows -= 2;
}

void editorFreeRow(erow *row) {
  free(row->render);
  free(row->chars);
  free(row->hl);
}
void editorDelRow(int at) {
  if (at < 0 || at >= E.numrows) return;
  editorFreeRow(&E.row[at]);
  memmove(&E.row[at], &E.row[at + 1], sizeof(erow) * (E.numrows - at - 1));
  
  // when delete row, we need to update row index after it.
  for (int j = at; j < E.numrows - 1; j++) E.row[j].idx--;
  E.numrows--;
  E.dirty++;
}

/**
 * @brief 
 * 
 * @param row 
 * @param at 
 * @param c 
 */
void editorRowInsertChar(erow *row, int at, int c) {
  if (at < 0 || at > row->size) at = row->size;
  row->chars = realloc(row->chars, row->size + 2);
  memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1);
  row->size++;
  row->chars[at] = c;
  editorUpdateRow(row);
  E.dirty ++;
}

void editorRowAppendString(erow *row, char *s, size_t len) {
  row->chars = realloc(row->chars, row->size + len + 1);
  memcpy(&row->chars[row->size], s, len);
  row->size += len;
  row->chars[row->size] = '\0';
  editorUpdateRow(row);
  E.dirty++;
}

void editorRowDelChar(erow *row, int at) {
  if (at < 0 || at >= row->size) return;
  memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
  row->size--;
  editorUpdateRow(row);
  E.dirty++;
}


/**
 * @brief 
 * 
 * @param c 
 */
void editorInsertChar(int c) {
  if (E.cy == E.numrows) {
    editorInsertRow(E.numrows, "", 0);
  }
  editorRowInsertChar(&E.row[E.cy], E.cx, c);
  E.cx++;
  E.dirty++;
}

void editorInsertNewline() {
  if (E.cx == 0) {
    editorInsertRow(E.cy, "", 0);
  } else {
    erow *row = &E.row[E.cy];
    editorInsertRow(E.cy + 1, &row->chars[E.cx], row->size - E.cx);
    row = &E.row[E.cy];
    row->size = E.cx;
    row->chars[row->size] = '\0';
    editorUpdateRow(row);
  }
  E.cy++;
  E.cx = 0;
}

void editorDelChar() {
  if (E.cy == E.numrows) return;
  if (E.cx == 0 && E.cy == 0) return;
  erow *row = &E.row[E.cy];
  if (E.cx > 0) {
    editorRowDelChar(row, E.cx - 1);
    E.cx--;
  } else {
    E.cx = E.row[E.cy - 1].size;
    editorRowAppendString(&E.row[E.cy - 1], row->chars, row->size);
    editorDelRow(E.cy);
    E.cy--;
  }
}


void editorSelectSyntaxHighlight() {
  E.syntax = NULL;
  if (E.filename == NULL) return;
  char *ext = strrchr(E.filename, '.');
  for (unsigned int j = 0; j < HLDB_ENTRIES; j++) {
    struct editorSyntax *s = &HLDB[j];
    unsigned int i = 0;
    while (s->filematch[i]) {
      int is_ext = (s->filematch[i][0] == '.');
      if ((is_ext && ext && !strcmp(ext, s->filematch[i])) ||
          (!is_ext && strstr(E.filename, s->filematch[i]))) {
        E.syntax = s;

        int filerow;
        for (filerow = 0; filerow < E.numrows; filerow++) {
          editorUpdateSyntax(&E.row[filerow]);
        }

        return;
      }
      i++;
    }
  }
}
