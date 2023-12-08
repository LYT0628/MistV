#ifndef __BUFFER_H__
#define __BUFFER_H__

struct abuf {
  char *b;
  int len;
};

#define ABUF_INIT {NULL, 0}

#endif