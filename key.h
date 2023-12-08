#ifndef __KEY_H__
#define __KEY_H__


enum editorKey {
  ARROW_LEFT = 1000,
  ARROW_RIGHT,
  ARROW_UP,
  ARROW_DOWN,
  DEL_KEY,
  HOME_KEY,
  END_KEY,
  PAGE_UP,
  PAGE_DOWN,
};



char editorReadKey();
void editorProcessKeypress() ;
#endif