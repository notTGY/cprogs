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
 * Local matrix-vector multiplication: result_local = A_local * v_local
 * A_local is n_local_rows x n_local_cols, v_local is n_local_cols, result_local is n_local_rows
 */
void local_matvec(const double *A_local, const double *v_local, const int n_local_rows, const int n_local_cols, double *result_local)
{
    for (int i = 0; i < n_local_rows; i++) {
        double sum = 0.0;
        for (int j = 0; j < n_local_cols; j++) {
            sum += A_local[i * n_local_cols + j] * v_local[j];
        }
        result_local[i] = sum;
    }
}

/**
 * Parallel CGM with block decomposition: x_local = A^-1 * b_local
 */
void par_cgm(const double *A_local, const double *b_local, const int n, const int n_local_rows, const int n_local_cols,
             int *max_iter, double *tol, double *x_local, MPI_Comm row_comm, MPI_Comm col_comm)
{
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double *r_local = (double*)malloc(sizeof(double) * n_local_rows);
    double *p_local = (double*)malloc(sizeof(double) * n_local_rows);
    double *ap_local = (double*)malloc(sizeof(double) * n_local_rows);
    double *temp_result = (double*)malloc(sizeof(double) * n_local_rows);

    // Initial residual: r = b - A * x
    local_matvec(A_local, x_local, n_local_rows, n_local_cols, temp_result);
    MPI_Reduce(temp_result, r_local, n_local_rows, MPI_DOUBLE, MPI_SUM, 0, col_comm);
    if (MPI_Comm_rank(col_comm, &rank) == 0) {
        for (int i = 0; i < n_local_rows; i++) {
            r_local[i] = b_local[i] - r_local[i];
        }
    }
    MPI_Bcast(r_local, n_local_rows, MPI_DOUBLE, 0, col_comm);
    copyv(r_local, n_local_rows, p_local);
    double rr = par_dot(r_local, r_local, n_local_rows, row_comm);

    int k;
    for (k = 0; k < *max_iter; k++) {
        local_matvec(A_local, p_local, n_local_rows, n_local_cols, temp_result);
        MPI_Reduce(temp_result, ap_local, n_local_rows, MPI_DOUBLE, MPI_SUM, 0, col_comm);
        MPI_Bcast(ap_local, n_local_rows, MPI_DOUBLE, 0, col_comm);
        double p_dot_ap = par_dot(p_local, ap_local, n_local_rows, row_comm);
        if (p_dot_ap == 0.0) {
            if (rank == 0) printf("Warning: p^T A p = 0, CGM may diverge\n");
            break;
        }
        double alpha = rr / p_dot_ap;
        addv(1.0, x_local, alpha, p_local, n_local_rows, x_local);
        addv(1.0, r_local, -alpha, ap_local, n_local_rows, r_local);
        double newrr = par_dot(r_local, r_local, n_local_rows, row_comm);
        if (sqrt(newrr) < *tol) {
            rr = newrr;
            k++;
            break;
        }
        addv(1.0, r_local, newrr / rr, p_local, n_local_rows, p_local);
        rr = newrr;
    }
    *max_iter = k;
    *tol = sqrt(rr);

    free(r_local);
    free(p_local);
    free(ap_local);
    free(temp_result);
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
    int grid_size = (int)sqrt((double)size);
    if (grid_size * grid_size != size) {
        if (rank == 0) {
            printf("Number of processes must be a perfect square.\n");
        }
        MPI_Finalize();
        return 1;
    }
    if (N % grid_size != 0) {
        if (rank == 0) {
            printf("N must be divisible by sqrt(p).\n");
        }
        MPI_Finalize();
        return 1;
    }
    int n_local = N / grid_size;

    // Set up 2D process grid
    int coords[2];
    MPI_Comm cart_comm;
    int dims[2] = {grid_size, grid_size};
    int periods[2] = {0, 0};
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cart_comm);
    MPI_Cart_coords(cart_comm, rank, 2, coords);
    int row_rank = coords[0];
    int col_rank = coords[1];

    // Create row and column communicators
    MPI_Comm row_comm, col_comm;
    MPI_Comm_split(MPI_COMM_WORLD, row_rank, col_rank, &row_comm);
    MPI_Comm_split(MPI_COMM_WORLD, col_rank, row_rank, &col_comm);

    // Generate full R (N x N) locally to compute A_local block
    double *R = (double*)malloc(sizeof(double) * N * N);
    srand(0);
    for (int i = 0; i < N * N; i++) {
        R[i] = (double)rand() / RAND_MAX * 10.0;
    }

    // Compute A_local = R * R^T (n_local x n_local) for local block
    double *A_local = (double*)malloc(sizeof(double) * n_local * n_local);
    for (int i_local = 0; i_local < n_local; i_local++) {
        int global_i = row_rank * n_local + i_local;
        for (int j_local = 0; j_local < n_local; j_local++) {
            int global_j = col_rank * n_local + j_local;
            double sum = 0.0;
            for (int k = 0; k < N; k++) {
                sum += R[global_i * N + k] * R[global_j * N + k];
            }
            A_local[i_local * n_local + j_local] = sum;
        }
    }

    // Add 1 to diagonal elements
    if (row_rank == col_rank) {
        for (int i_local = 0; i_local < n_local; i_local++) {
            int global_i = row_rank * n_local + i_local;
            if (global_i == col_rank * n_local + i_local) {
                A_local[i_local * n_local + i_local] += 1.0;
            }
        }
    }

    // Initialize b_local and x_local
    double *b_local = (double*)calloc(n_local, sizeof(double));
    double *x_local = (double*)calloc(n_local, sizeof(double));

    // Compute b_local = A_local * ones(N) for first column of grid
    if (col_rank == 0) {
        for (int i_local = 0; i_local < n_local; i_local++) {
            for (int j_local = 0; j_local < n_local; j_local++) {
                b_local[i_local] += A_local[i_local * n_local + j_local];
            }
        }
    }

    // Broadcast b_local across row communicator
    MPI_Bcast(b_local, n_local, MPI_DOUBLE, 0, row_comm);

    // Set CGM parameters
    int max_iter = N;
    double tol = 1e-8;
    if (argc > 2) tol = atof(argv[2]);
    if (argc > 3) max_iter = atoi(argv[3]);
    MPI_Bcast(&max_iter, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&tol, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Run parallel CGM
    par_cgm(A_local, b_local, N, n_local, n_local, &max_iter, &tol, x_local, row_comm, col_comm);



    // Output results (commented out as in original)
    // Gather solution on rank 0
    /*
    double *x = NULL;
    if (rank == 0) x = (double*)malloc(sizeof(double) * N);
    double *x_block = (double*)malloc(sizeof(double) * n_local);
    if (col_rank == 0) {
        memcpy(x_block, x_local, n_local * sizeof(double));
    } else {
        memset(x_block, 0, n_local * sizeof(double));
    }
    MPI_Gather(x_block, n_local, MPI_DOUBLE, x, n_local, MPI_DOUBLE, 0, col_comm);
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
    free(x_block);
    if (rank == 0) free(x);
    */

    // Clean up
    free(R);
    free(A_local);
    free(b_local);
    free(x_local);
    MPI_Comm_free(&row_comm);
    MPI_Comm_free(&col_comm);
    MPI_Comm_free(&cart_comm);

    MPI_Finalize();
    return 0;
}
