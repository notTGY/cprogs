FLAGS=-fprofile-arcs -ftest-coverage

all: main.exe

main.exe: main.o lib/hash-table.o
	gcc lib/hash-table.o main.o -o main.exe ${FLAGS}

main.o: main.c
	gcc -c main.c -o main.o ${FLAGS}

lib/hash-table.o: lib/hash-table.c
	gcc -c lib/hash-table.c -o lib/hash-table.o ${FLAGS}

clean:
	rm *.exe && rm **/*.o && rm *.o && rm *.gcda && rm *.gcno && rm **/*.gcda && rm **/*.gcno
