calc : main.o sum.o sub.o mul.o div.o
	gcc -o main.o sum.o sub.o mul.o div.o

main.o : main.c ./lib
	gcc -c./lib/main.c

sum.o : sum.c
	gcc -c ./lib/sum.c

sub.o : sub.c
	gcc -c ./lib/sub.c

mul.o : mul.c
	gcc -c ./lib/mul.c

div.o : div.c
	gcc -c ./lib/div.c

clean :
	rm -f main.o sum.o sub.o mul.o div.o
