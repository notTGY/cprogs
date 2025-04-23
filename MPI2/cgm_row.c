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
    double res = 0.0;
    for (int i = 0; i < n_local; i++) {
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
    for (int i = 0; i < n; i++) {
        c[i] = alpha * a[i] + beta * b[i];
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
    for (int i = 0; i < n_local; i++) {
        double sum = 0.0;
        for (int j = 0; j < n; j++) {
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
 * Parallel CGM: x_local = A^-1 * b_local
 */
void par_cgm(const double *A_local, const double *b_local, const int n, const int n_local, 
             int *max_iter, double *tol, double *x_local, MPI_Comm comm)
{
    double *r_local = (double*)malloc(sizeof(double) * n_local);
    double *p_local = (double*)malloc(sizeof(double) * n_local);
    double *ap_local = (double*)malloc(sizeof(double) * n_local);
    double *v_full = (double*)malloc(sizeof(double) * n);

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

    if (argc < 2) {
        if (rank == 0) {
            printf("Usage: %s N tolerance(default 1e-8) max_iter(default is N)\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    int N = atoi(argv[1]);
    if (N % size != 0) {
        if (rank == 0) {
            printf("N must be divisible by the number of processes.\n");
        }
        MPI_Finalize();
        return 1;
    }
    int m = N / size;

    // Allocate local R and temporary R for broadcasting
    double *R_local = (double*)malloc(sizeof(double) * m * N);
    double *R_temp = (double*)malloc(sizeof(double) * m * N);

    // Generate local R with deterministic seed
    srand(0);
    for (int skip = 0; skip < rank * m * N; skip++) {
        rand();
    }
    for (int i_local = 0; i_local < m; i_local++) {
        for (int j = 0; j < N; j++) {
            R_local[i_local * N + j] = (double)rand() / RAND_MAX * 10.0;
        }
    }

    // Allocate A_local
    double *A_local = (double*)malloc(sizeof(double) * m * N);

    // Compute A_local = R_local * R^T using broadcasts
    for (int proc = 0; proc < size; proc++) {
        if (rank == proc) {
            memcpy(R_temp, R_local, sizeof(double) * m * N);
        }
        MPI_Bcast(R_temp, m * N, MPI_DOUBLE, proc, MPI_COMM_WORLD);
        for (int i_local = 0; i_local < m; i_local++) {
            for (int j_local = 0; j_local < m; j_local++) {
                double sum = 0.0;
                for (int l = 0; l < N; l++) {
                    sum += R_local[i_local * N + l] * R_temp[j_local * N + l];
                }
                A_local[i_local * N + (proc * m + j_local)] = sum;
            }
        }
    }

    // Add 1 to diagonal elements
    for (int i_local = 0; i_local < m; i_local++) {
        int global_i = rank * m + i_local;
        A_local[i_local * N + global_i] += 1.0;
    }

    // Compute b_local = A_local * ones(N)
    double *b_local = (double*)malloc(sizeof(double) * m);
    for (int i_local = 0; i_local < m; i_local++) {
        b_local[i_local] = 0.0;
        for (int j = 0; j < N; j++) {
            b_local[i_local] += A_local[i_local * N + j];
        }
    }

    // Initialize x_local
    double *x_local = (double*)calloc(m, sizeof(double));

    // Set CGM parameters
    int max_iter = N;
    double tol = 1e-8;
    if (argc > 2) tol = atof(argv[2]);
    if (argc > 3) max_iter = atoi(argv[3]);
    MPI_Bcast(&max_iter, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&tol, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Run parallel CGM
    par_cgm(A_local, b_local, N, m, &max_iter, &tol, x_local, MPI_COMM_WORLD);

    // Gather solution on rank 0
    double *x = NULL;
    if (rank == 0) x = (double*)malloc(sizeof(double) * N);
    MPI_Gather(x_local, m, MPI_DOUBLE, x, m, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Output results
    /*
    if (rank == 0) {
        for (int i = 0; i < N; i++) printf("%f ", x[i]);
        printf("\n");
        printf("tol %e\n", tol);
        printf("max_iter %d\n", max_iter);
    }
    */

    // Clean up
    free(R_local);
    free(R_temp);
    free(A_local);
    free(b_local);
    free(x_local);
    if (rank == 0) free(x);

    MPI_Finalize();
    return 0;
}
