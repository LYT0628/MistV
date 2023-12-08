#ifndef __TERMINAL_H__
#define __TERMINAL_H__

// transform a char to a ctrl+<char>
#define CTRL_KEY(k) ((k) & 0x1f)

void enableRawMode();
void editorRefreshScreen();
void initEditor();
int getWindowSize(int *rows, int *cols);


#endif