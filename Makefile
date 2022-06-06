
all: myshell shell3 test


myshell: myshell.c
	gcc -o myshell myshell.c

test: test.c
	gcc -o test test.c

# shell1: shell1.c
# 	gcc -o shell1 shell1.c

# shell2: shell2.c
# 	gcc -o shell2 shell2.c

shell3: shell3.c
	gcc -o shell3 shell3.c

clean:
	rm -f *.o shell1 shell2 shell3 myshell test