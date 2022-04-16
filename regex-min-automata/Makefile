FLAGS=-fprofile-arcs -ftest-coverage

all: a.exe

a.exe: common.o AST.o Automata.o minimize.o main.o
	g++ common.o AST.o Automata.o minimize.o main.o -o a.exe ${FLAGS}

main.o: main.cpp
	g++ -c main.cpp -o main.o ${FLAGS}

Automata.o: Automata.cpp
	g++ -c Automata.cpp -o Automata.o ${FLAGS}

AST.o: AST.cpp
	g++ -c AST.cpp -o AST.o ${FLAGS}

common.o: common.cpp
	g++ -c common.cpp -o common.o ${FLAGS}

minimize.o: minimize.cpp
	g++ -c minimize.cpp -o minimize.o ${FLAGS}

clean:
	rm *.exe && rm **/*.o && rm *.o && rm *.gcda && rm *.gcno && rm **/*.gcda && rm **/*.gcno
