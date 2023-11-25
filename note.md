

## C语言知识
```c

//local flags, iflag-input,oflag-output,cglag-control-flag
// ICANON: reading line-by-line, and now byte-by-byte

```

## Makefile知识
```makefile
kilo： kilo.c
	$(cc) kilo.c -o kilo -Wall -Wextra -pedantic -std=c99

# $(cc): 指向C语言编译器，一般是gcc
# -Wall： 显示警告，all Warnings
# -Wextra -pedantic:显示更多的警告 ，包括“未使用的变量”
# -std=c99: 使用 ANSIC 标准


```

## shell知识
```shell

echo $?
# print the return value of main

```
