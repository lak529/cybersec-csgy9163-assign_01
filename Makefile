CC=afl-gcc
default: prog

get-deps:
	# Assuming Debian or Ubuntu here
	sudo apt-get install -y build-essential check

dictionary.o: dictionary.c
	$(CC) -Wall -c dictionary.c dictionary.h

spell.o: spell.c
	$(CC) -Wall -g -c spell.c

test.o: test_main.c
	$(CC) -Wall -c test_main.c

main.o: main.c
	$(CC) -Wall -g -c main.c

test: dictionary.o spell.o test_main.o
	$(CC) -Wall -o test_main test_main.o spell.o dictionary.o -lcheck -lm -lrt -lpthread -lsubunit
	./test_main

prog: dictionary.o spell.o main.o
	$(CC) -Wall -g -o spell_check dictionary.o spell.o main.o

afl: dictionary.o spell.o main.o
	$(CC) -Wall -g -o spell_check dictionary.o spell.o main.o

clean:
	rm dictionary.o spell.o main.o test_main.o check_spell.o

cleanall:clean
	rm spell_check
	
