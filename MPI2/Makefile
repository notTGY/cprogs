task:
	gcc -fopenmp tasks.c && time ./a.out 10000000
mmul:
	mpicc mmul.c && mpiexec -n 1 ./a.out
merge:
	gcc -fopenmp merge_sort.c && ./a.out 30
fib:
	gcc -fopenmp section.c && ./a.out 30
