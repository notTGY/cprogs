#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/**
 * (a,b)
 */
double dot(const double *a, const double *b, const int n)
{
	int i;
	double res = 0.0;
	for (i = 0; i < n; i++) {
		res += a[i] * b[i];
	}
	return res;
}

/**
 * c = alpha * a + beta * b
 */
void addv(const double alpha, const double *a, const double beta, const double *b, const int n, double *c)
{
	int i;
	for (i = 0; i < n; i++) {
		c[i] = alpha * a[i] + beta * b[i];
	}
}

/**
 * d = A * x
 */
void matvec(const double *A, const double *x, const int n, double *b)
{
	int i;
	for (i = 0; i < n; i++) {
		b[i] = dot(x, A + i * n, n);
	}
}


/**
 * b = a
 */
void copyv(const double *a, const int n, double *b)
{
	memcpy(b, a, sizeof(double) * n);
}

/**
 * Read matrix from file:
 * M N
 * a11 a12 ..... a1N
 * a21 a22 ..... a2N
 * .....
 * aM1 ......... aMN
 */
void read_matrix(const char *path, double **A, int *m, int *n)
{
	int i, j;
	FILE *f = fopen(path, "r");
	fscanf(f, "%d", m);
	fscanf(f, "%d", n);
	double *t = (double*)malloc(sizeof(double) * (*m) * (*n));
	for (j = 0; j < *m; j++) {
		for (i = 0; i < *n; i++) {
			float fl;
			fscanf(f, "%f", &fl); 
			t[i + (*n) * j] = fl;
		}
	}
	close(f);
	*A = t;
}

/**
 * x = A^-1 * b
 */
void cgm(const double *A, const double *b, const int n, int *max_iter, double *tol, double *x)
{
	double *r = (double*)malloc(sizeof(double) * n);
	double *p = (double*)malloc(sizeof(double) * n);
	double *ap = (double*)malloc(sizeof(double) * n);
	
	matvec(A, x, n, r);
	addv(1.0, b, -1.0, r, n, r);
	copyv(r, n, p);
	double rr = dot(r, r, n);
	
	int k;
	for (k = 0; k < *max_iter; k++) {
		matvec(A, p, n, ap);
		double alpha = rr / dot(p, ap, n);
		addv(1.0, x, alpha, p, n, x);
		addv(1.0, r, -alpha, ap, n, r);
		double newrr = dot(r, r, n);
		if (sqrt(newrr) < *tol) {
			rr = newrr;
			k++;
			break;
		}
		addv(1.0, r, newrr / rr, p, n, p);
		rr = newrr;
	}
	*max_iter = k;
	*tol = sqrt(rr);
	free(r);
	free(p);
	free(ap);
}

int main(int argc, char **argv)
{
	if (argc < 3) {
		printf("Usage: %s A b tolerance(default 1e-8) max_iter(default is b dimension)\n", argv[0]);
		exit(1);
	}
	int N, M;
	double *A;
	double *b;
	double *x;
	read_matrix(argv[1], &A, &M, &N);
	if (M != N) {
		printf("Only square matrix are supported. Current matrix %dx%d.\n", M, N);
		exit(1);
	}
	int N1, M1;
	read_matrix(argv[2], &b, &M1, &N1);
	if (M1 * N1 != N) {
		printf("Invalid b size %d.\n", M1 * N1);
		exit(1);
	}
	x = (double*)calloc(N, sizeof(double));
	
	int max_iter = N;
	double tol = 1e-8;
	if (argc > 4) {
		max_iter = atoi(argv[4]);
	}
	if (argc > 3) {
		tol = atof(argv[3]);
	}
	
	cgm(A, b, N, &max_iter, &tol, x);
	
	int i;
	for (i = 0; i < N; i++) {
		printf("%f ", x[i]);
	}
	printf("\n");
	printf("tol %e\n", tol);
	printf("max_iter %d\n", max_iter);
	
	double *t = (double*)calloc(N, sizeof(double));
	matvec(A, x, N, t);
	addv(1.0, t, -1.0, b, N, t);
	printf("||A*x-b|| %e\n", sqrt(dot(t, t, N)));
	
	free(t);
	free(A);
	free(b);
	free(x);
	return 0;
}
