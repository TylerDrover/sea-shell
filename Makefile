CFLAGS = -Wall -Werror

main: shell.c 
	gcc $(CFLAGS) -o shell shell.c

clean:
	-rm shell

run:
	make
	clear
	./shell
