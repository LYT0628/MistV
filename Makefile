kilo: kilo.c
	cc -o $@ $^ -Wall -Wextra -pedantic  -std=c99

.Proxy: clean
clean:
	rm -rf kilo
