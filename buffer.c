// realloc() and free() come from <stdlib.h>. memcpy() comes from <string.h>.
#include <string.h>
#include <stdlib.h>

#include "buffer.h"

/**
 * @brief 
 * 
 * @param ab 
 * @param s 
 * @param len 
 */
void abAppend(struct abuf *ab, const char *s, int len) {
  char *new = realloc(ab->b, ab->len + len);
  if (new == NULL) return;
  memcpy(&new[ab->len], s, len);
  ab->b = new;
  ab->len += len;
}

/**
 * @brief 
 * 
 * @param ab 
 */
void abFree(struct abuf *ab) {
  free(ab->b);
}