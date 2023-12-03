#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>


struct termios orig_termios;


void die(const char *s) {
  perror(s);
  exit(1);
}

// reset termios, disable raw mode and echo appear
void disableRawMode(){
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios))
    die("tcsetattr");
}

// enable raw mode and no echo appear
void enableRawMode(){
	// save original setting
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
	// register callback when exit called
	//is function comes from stdio.h
	atexit(disableRawMode);	
	
	
	struct termios raw = orig_termios;
	
	// termios attr, getter to raw	struct
	tcgetattr(STDIN_FILENO, &raw);
	
	//c_lflag: local flags ， tag for various attr
	// c_iflag, c_oflag, c_cflag are for input, output, and control 
	// turn off echo and icanon mode( from line-line reading to byte-byte reading)
	// turn off ISIG(eg. ctrl+z, ctrl+y), IXON(eg. ctrl+s, ctrl + q), IEXTEN(eg. ctrl+v), ICRNL(ctrl+m). see: https://en.wikipedia.org/wiki/Software_flow_control
  	raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
  	// turn off output process
  	raw.c_oflag &= ~(OPOST);
  	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	
	
  	raw.c_cc[VMIN] = 0;// at least 0 char
  	raw.c_cc[VTIME] = 1;// max wait time
	
	// setter , when all output is ready
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

int main(){
	enableRawMode();
	while(1){
		char c = '\0';
    if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
		if(iscntrl(c)){ // is a control character(0-31, 127).see: https://www.asciitable.com/
			printf("%d\r\n", c);// cause disable OPOSE, we should add \r for pretty look
		}else{
			printf("%d('%c')\r\n", c,c);
		}
		if (c == 'q') break;
	}// while(1)
	return 0;
}

