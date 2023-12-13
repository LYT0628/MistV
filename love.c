#define _DEFAULT_SOURCE
#define _BSD_SOURCE //
#define _GNU_SOURCE

       
#include "key.h"
#include "terminal.h"
#include "editor.h"



int main(int argc, char *argv[]) {
  enableRawMode();
  initEditor();
  if (argc >= 2) {
    editorOpen(argv[1]);
  }

  editorSetStatusMessage("HELP: Ctrl-S = save | Ctrl-Q = quit");

  while (1) {
    editorRefreshScreen();
    editorProcessKeypress();
  }

  return 0;
}
