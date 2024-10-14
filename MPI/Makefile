3: 3.out
	mpiexec -n 2 ./3.out ./life/conf1.cfg
3.out: 3.c
	mpicc 3.c -lm -o 3.out

2: 2.out
	mpiexec -n 2 ./2.out
2.out: 2.c
	mpicc 2.c -lm -o 2.out

1: 1.out
	mpiexec -n 1 ./1.out 10000
1.out: 1.c
	mpicc 1.c -lm -o 1.out
