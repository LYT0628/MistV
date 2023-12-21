# kilo: kilo.c escape.h error.c error.h terminal.c terminal.h
# 	cc -o $@ $^ -Wall -Wextra -pedantic  -std=c99

mistv: mistv.c terminal.c terminal.h error.c error.h key.c key.h escape.h  editor.c editor.h buffer.c buffer.h search.c search.h highlight.c highlight.h
	cc  -o  $@ $^ -Wall -Wextra -pedantic  -std=c99

.Proxy: clean
clean:
	rm -rf love
