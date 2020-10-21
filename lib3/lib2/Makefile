calc : main.o sum.o sub.o mul.o div.o
	gcc -o calc main.o sum.o sub.o mul.o div.o

main.o : main.c
	gcc -c main.c

sum.o : sum.c
	gcc -c sum.c

sub.o : sub.c
	gcc -c sub.c

mul.o : mul.c
	gcc -c mul.c

div.o : div.c
	gcc -c div.c

clean :
	rm -f calc main.o sum.o sub.o mul.o div.o
