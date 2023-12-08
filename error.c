/*** include ***/
#include <unistd.h>
// write() and STDOUT_FILENO come from <unistd.h>.

#include <errno.h>
// perror()
#include <stdlib.h>

#include "escape.h"


void die(const char *s) {
      write(STDOUT_FILENO, ES_CLEAR_ENTIRE_SCREEN, ES_CLEAR_ENTIRE_SCREEN_SIZE);
      write(STDOUT_FILENO, ES_POSITION_CURSOR_ORIGIN, ES_POSITION_CURSOR_ORIGIN_SIZE);

  perror(s);
  exit(1);
}
