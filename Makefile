%.o: %.c
	clang -c -Wall -Werror $^

hashtable: hashtable.o main.o cli.o
	clang -lreadline -o hashtable $^

server: hashtable.o serve.o cli.o
	clang -lreadline -o server $^

clean:
	rm *.o hashtable server

valgrind: hashtable
	valgrind --suppressions=valgrind_suppr ./hashtable
