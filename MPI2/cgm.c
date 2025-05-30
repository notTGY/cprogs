#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

/**
 * Local dot product for a portion of vectors a and b
 */
double local_dot(const double *a, const double *b, const int n_local)
{
    int i;
    double res = 0.0;
    for (i = 0; i < n_local; i++) {
        res += a[i] * b[i];
    }
    return res;
}

/**
 * Global dot product using MPI_Allreduce
 */
double par_dot(const double *a_local, const double *b_local, const int n_local, MPI_Comm comm)
{
    double local_sum = local_dot(a_local, b_local, n_local);
    double global_sum;
    MPI_Allreduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, comm);
    return global_sum;
}

/**
 * c = alpha * a + beta * b (local operation)
 */
void addv(const double alpha, const double *a, const double beta, const double *b, const int n, double *c)
{
    int i;
    for (i = 0; i < n; i++) {
        c[i] = alpha *a[i] + beta * b[i];
    }
}

/**
 * b = a (local operation)
 */
void copyv(const double *a, const int n, double *b)
{
    memcpy(b, a, sizeof(double) * n);
}

/**
 * Local matrix-vector multiplication: result_local = A_local * v_full
 */
void local_matvec(const double *A_local, const double *v_full, const int n_local, const int n, double *result_local)
{
    int i, j;
    for (i = 0; i < n_local; i++) {
        double sum = 0.0;
        for (j = 0; j < n; j++) {
            sum += A_local[i * n + j] * v_full[j];
        }
        result_local[i] = sum;
    }
}

/**
 * Gather the distributed vector v_local into v_full on all processes
 */
void gather_vector(const double *v_local, const int n_local, const int n, MPI_Comm comm, double *v_full)
{
    MPI_Allgather(v_local, n_local, MPI_DOUBLE, v_full, n_local, MPI_DOUBLE, comm);
}

/**
 * Read matrix from file on rank 0
 */
void read_matrix(const char *path, double **A, int *m, int *n)
{
    int i, j;
    FILE *f = fopen(path, "r");
    if (!f) {
        perror("Cannot open file");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
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
    fclose(f);
    *A = t;
}

/**
 * Parallel CGM: x_local = A^-1 * b_local
 */
void par_cgm(const double *A_local, const double *b_local, const int n, const int n_local, 
             int *max_iter, double *tol, double *x_local, MPI_Comm comm)
{
    double *r_local = (double*)malloc(sizeof(double) * n_local);
    double *p_local = (double*)malloc(sizeof(double) * n_local);
    double *ap_local = (double*)malloc(sizeof(double) * n_local);
    double *v_full = (double*)malloc(sizeof(double) * n);  // Full vector for matvec

    // Initial residual: r = b - A * x
    gather_vector(x_local, n_local, n, comm, v_full);
    local_matvec(A_local, v_full, n_local, n, r_local);
    addv(1.0, b_local, -1.0, r_local, n_local, r_local);
    copyv(r_local, n_local, p_local);
    double rr = par_dot(r_local, r_local, n_local, comm);

    int k;
    for (k = 0; k < *max_iter; k++) {
        gather_vector(p_local, n_local, n, comm, v_full);
        local_matvec(A_local, v_full, n_local, n, ap_local);
        double p_dot_ap = par_dot(p_local, ap_local, n_local, comm);
        double alpha = rr / p_dot_ap;
        addv(1.0, x_local, alpha, p_local, n_local, x_local);
        addv(1.0, r_local, -alpha, ap_local, n_local, r_local);
        double newrr = par_dot(r_local, r_local, n_local, comm);
        if (sqrt(newrr) < *tol) {
            rr = newrr;
            k++;
            break;
        }
        addv(1.0, r_local, newrr / rr, p_local, n_local, p_local);
        rr = newrr;
    }
    *max_iter = k;
    *tol = sqrt(rr);

    free(r_local);
    free(p_local);
    free(ap_local);
    free(v_full);
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 3) {
        if (rank == 0) {
            printf("Usage: %s A b tolerance(default 1e-8) max_iter(default is b dimension)\n", argv[0]);
        }
        MPI_Finalize();
        exit(1);
    }

    int N, M;
    double *A = NULL, *b = NULL;
    double *A_local, *b_local, *x_local;

    // Read matrix and vector on rank 0
    if (rank == 0) {
        read_matrix(argv[1], &A, &M, &N);
        if (M != N) {
            printf("Only square matrices are supported. Current matrix %dx%d.\n", M, N);
            free(A);
            MPI_Finalize();
            exit(1);
        }
        read_matrix(argv[2], &b, &M, &N);
        if (M * N != N) {
            printf("Invalid b size %d.\n", M * N);
            free(A);
            free(b);
            MPI_Finalize();
            exit(1);
        }
    }

    // Broadcast N to all processes
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Assuming N is divisible by size for simplicity
    int n_local = N / size;
    A_local = (double*)malloc(sizeof(double) * n_local * N);
    b_local = (double*)malloc(sizeof(double) * n_local);
    x_local = (double*)calloc(n_local, sizeof(double));

    // Distribute A and b
    MPI_Scatter(A, n_local * N, MPI_DOUBLE, A_local, n_local * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(b, n_local, MPI_DOUBLE, b_local, n_local, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    int max_iter = N;
    double tol = 1e-8;
    if (argc > 4 && rank == 0) {
        max_iter = atoi(argv[4]);
    }
    if (argc > 3 && rank == 0) {
        tol = atof(argv[3]);
    }
    MPI_Bcast(&max_iter, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&tol, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    par_cgm(A_local, b_local, N, n_local, &max_iter, &tol, x_local, MPI_COMM_WORLD);

    // Gather solution x on rank 0
    double *x = NULL;
    if (rank == 0) {
        x = (double*)malloc(sizeof(double) * N);
    }
    MPI_Gather(x_local, n_local, MPI_DOUBLE, x, n_local, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Output results on rank 0
    if (rank == 0) {
        int i;
        for (i = 0; i < N; i++) {
            printf("%f ", x[i]);
        }
        printf("\n");
        printf("tol %e\n", tol);
        printf("max_iter %d\n", max_iter);

        double *t = (double*)calloc(N, sizeof(double));
        double *Ax = (double*)calloc(N, sizeof(double));
        for (i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                Ax[i] += A[i * N + j] * x[j];
            }
            t[i] = Ax[i] - b[i];
        }
        double res_norm = sqrt(local_dot(t, t, N));
        printf("||A*x-b|| %e\n", res_norm);
        free(t);
        free(Ax);
        free(x);
        free(A);
        free(b);
    }

    free(A_local);
    free(b_local);
    free(x_local);

    MPI_Finalize();
    return 0;
}
