# kilo: kilo.c escape.h error.c error.h terminal.c terminal.h
# 	cc -o $@ $^ -Wall -Wextra -pedantic  -std=c99

# shell 路径
SHELL		= /bin/sh
# Makefile路口
THIS		= Makefile

# 中间文件后缀
c   = .c 
o		= .o


# 上级路径
top_srcdir	= ..
# 当前路径
SRC_DIR		= .
# 下载路径前缀
prefix		= /usr
# 执行文件前缀
exec_prefix	= ${prefix}
# 系统shell指令查找路径
bindir		= ${exec_prefix}/bin
# gcc -l 依赖查找路径
libdir		= ${exec_prefix}/lib

# 软件名称
APP_NAME		= mistv 


INSTALL		= /usr/bin/install 


CCOPTS = -Wall -Wextra -pedantic  -std=c99
CCOPTS += -l ncurses

INSTALL_DIR = /usr/bin


CC		= gcc
CPP		= gcc -E


SRCS = $(wildcard *.c) 
OBJS = $(SRCS:.c=.o)

mistv: $(OBJS)
	$(CC)  -o  $@ $^ $(CCOPTS)


.depend: $(SRCS)
	@rm -f .depend
		@$(foreach src, $(SRCS), \
				gcc - I. -MM $(src) >> .depend; \
		)
include .depend


.PHONY: clean install uninstall

clean: 
	rm -f mistv *.o 

install: 
	mv $(SRC_DIR)/$(APP_NAME)  $(INSTALL_DIR)/

uninstall: 
	rm  $(INSTALL_DIR)/$(APP_NAME)