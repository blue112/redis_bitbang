%.o: %.c
	gcc -c -Wall -Werror $^

hashtable: hashtable.o main.o
	gcc -lreadline -o hashtable $^

valgrind: hashtable
	valgrind --suppressions=valgrind_suppr ./hashtable
