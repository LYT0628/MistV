#ifndef __HIGHLIGHT_H__
#define __HIGHLIGHT_H__

#include "editor.h"
enum editorHighlight {
  HL_NORMAL = 0,
  HL_NUMBER
};

void editorUpdateSyntax(erow *row);

#endif
