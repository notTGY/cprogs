all: main.exe
main.exe: main.o lib/hash-table.o
	gcc lib/hash-table.o main.o -o main.exe
main.o: main.c
	gcc -c main.c -o main.o
lib/hash-table.o: lib/hash-table.c
	gcc -c lib/hash-table.c -o lib/hash-table.o
clean:
	rm **/*.o && rm *.o
