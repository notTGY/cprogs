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
 * Local matrix-vector multiplication: result_partial = A_local * v_local
 * A_local is N x n_local, v_local is n_local, result_partial is N
 */
void local_matvec(const double *A_local, const double *v_local, const int n_local, const int n, double *result_partial)
{
    for (int i = 0; i < n; i++) {
        double sum = 0.0;
        for (int j = 0; j < n_local; j++) {
            sum += A_local[i * n_local + j] * v_local[j];
        }
        result_partial[i] = sum;
    }
}

/**
 * Sum partial vectors using MPI_Allgather to get the full result vector
 */
void sum_partial_vectors(const double *partial_local, const int n_local, const int n, MPI_Comm comm, double *result_full)
{
    int size;
    MPI_Comm_size(comm, &size);
    double *recv_buf = (double*)malloc(sizeof(double) * n * size);
    MPI_Allgather(partial_local, n, MPI_DOUBLE, recv_buf, n, MPI_DOUBLE, comm);
    for (int i = 0; i < n; i++) {
        double sum = 0.0;
        for (int p = 0; p < size; p++) {
            sum += recv_buf[p * n + i];
        }
        result_full[i] = sum;
    }
    free(recv_buf);
}

/**
 * Parallel CGM: x_local = A^-1 * b_local
 */
void par_cgm(const double *A_local, const double *b_local, const int n, const int n_local, 
             int *max_iter, double *tol, double *x_local, MPI_Comm comm)
{
    int rank;
    MPI_Comm_rank(comm, &rank);
    double *r_local = (double*)malloc(sizeof(double) * n_local);
    double *p_local = (double*)malloc(sizeof(double) * n_local);
    double *ap_local = (double*)malloc(sizeof(double) * n_local);
    double *partial = (double*)malloc(sizeof(double) * n);
    double *v_full = (double*)malloc(sizeof(double) * n);

    // Initial residual: r = b - A * x
    local_matvec(A_local, x_local, n_local, n, partial);
    sum_partial_vectors(partial, n, n, comm, v_full);
    for (int i = 0; i < n_local; i++) {
        int global_i = rank * n_local + i;
        r_local[i] = b_local[i] - v_full[global_i];
    }
    copyv(r_local, n_local, p_local);
    double rr = par_dot(r_local, r_local, n_local, comm);

    int k;
    for (k = 0; k < *max_iter; k++) {
        local_matvec(A_local, p_local, n_local, n, partial);
        sum_partial_vectors(partial, n, n, comm, v_full);
        for (int i = 0; i < n_local; i++) {
            int global_i = rank * n_local + i;
            ap_local[i] = v_full[global_i];
        }
        double p_dot_ap = par_dot(p_local, ap_local, n_local, comm);
        if (p_dot_ap == 0.0) {
            if (rank == 0) printf("Warning: p^T A p = 0, CGM may diverge\n");
            break;
        }
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
    free(partial);
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
    int n_local = N / size;

    // Generate full R (N x N) locally to compute A_local columns
    double *R = (double*)malloc(sizeof(double) * N * N);
    srand(0);
    for (int i = 0; i < N * N; i++) {
        R[i] = (double)rand() / RAND_MAX * 10.0;
    }

    // Compute A_local = R * R^T (N x n_local) for local columns
    double *A_local = (double*)malloc(sizeof(double) * N * n_local);
    for (int i = 0; i < N; i++) {
        for (int j_local = 0; j_local < n_local; j_local++) {
            int global_j = rank * n_local + j_local;
            double sum = 0.0;
            for (int k = 0; k < N; k++) {
                sum += R[i * N + k] * R[global_j * N + k];
            }
            A_local[i * n_local + j_local] = sum;
        }
    }

    // Add 1 to diagonal elements
    for (int j_local = 0; j_local < n_local; j_local++) {
        int global_j = rank * n_local + j_local;
        A_local[global_j * n_local + j_local] += 1.0;
    }

    // Compute b_local = A_local * ones(N)
    double *b_local = (double*)malloc(sizeof(double) * n_local);
    for (int j_local = 0; j_local < n_local; j_local++) {
        b_local[j_local] = 0.0;
        for (int i = 0; i < N; i++) {
            b_local[j_local] += A_local[i * n_local + j_local];
        }
    }

    // Initialize x_local
    double *x_local = (double*)calloc(n_local, sizeof(double));

    // Set CGM parameters
    int max_iter = N;
    double tol = 1e-8;
    if (argc > 2) tol = atof(argv[2]);
    if (argc > 3) max_iter = atoi(argv[3]);
    MPI_Bcast(&max_iter, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&tol, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Run parallel CGM
    par_cgm(A_local, b_local, N, n_local, &max_iter, &tol, x_local, MPI_COMM_WORLD);

    // Gather solution on rank 0
    double *x = NULL;
    if (rank == 0) x = (double*)malloc(sizeof(double) * N);
    MPI_Gather(x_local, n_local, MPI_DOUBLE, x, n_local, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Output results
    /*
    if (rank == 0) {
        for (int i = 0; i < N; i++) printf("%f ", x[i]);
        printf("\n");
        printf("tol %e\n", tol);
        printf("max_iter %d\n", max_iter);

        // Compute residual ||A*x - b||
        double *Ax = (double*)calloc(N, sizeof(double));
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                double sum = 0.0;
                for (int k = 0; k < N; k++) {
                    sum += R[i * N + k] * R[j * N + k];
                }
                if (i == j) sum += 1.0;
                Ax[i] += sum * x[j];
            }
        }
        for (int i = 0; i < N; i++) {
            double b_i = 0.0;
            for (int k = 0; k < N; k++) {
                b_i += R[i * N + k] * R[i * N + k] + (i == k ? 1.0 : 0.0);
            }
            Ax[i] -= b_i;
        }
        double res_norm = sqrt(local_dot(Ax, Ax, N));
        printf("||A*x - b|| %e\n", res_norm);
        free(Ax);
    }
    */

    // Clean up
    free(R);
    free(A_local);
    free(b_local);
    free(x_local);
    if (rank == 0) free(x);

    MPI_Finalize();
    return 0;
}
