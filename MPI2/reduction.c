#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Function to compute the next power of 2
int next_power_of_2(int n) {
    return (int)pow(2, ceil(log2(n)));
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Problem size (example: N=29 as in the document)
    int N = 29;
    int q = next_power_of_2(N - 1); // Extend to power of 2

    // Allocate arrays for coefficients
    double *a = (double *)malloc((q + 1) * sizeof(double)); // Subdiagonal
    double *b = (double *)malloc((q + 1) * sizeof(double)); // Diagonal
    double *c = (double *)malloc((q + 1) * sizeof(double)); // Superdiagonal
    double *f = (double *)malloc((q + 1) * sizeof(double)); // Right-hand side
    double *x = (double *)malloc((q + 1) * sizeof(double)); // Solution

    // Initialize the system (example initialization)
    if (rank == 0) {
        for (int i = 1; i <= N; i++) {
            a[i] = 1.0; // Example subdiagonal
            b[i] = 4.0; // Example diagonal
            c[i] = 1.0; // Example superdiagonal
            f[i] = 100.0; // Example RHS
        }
        // Algorithm 2: System extension
        for (int j = N + 1; j <= q; j++) {
            a[j] = 0.0;
            c[j] = 0.0;
            f[j] = 0.0;
            b[j] = 1.0;
        }
        // Boundary conditions
        x[0] = 0.0;
        x[q] = 0.0;
    }

    // Broadcast initial data to all processes
    MPI_Bcast(a, q + 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(b, q + 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(c, q + 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(f, q + 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(x, q + 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Algorithm 3: Forward Reduction
    for (int s = 1; s <= q / 2; s *= 2) {
        // Compute number of tasks for this step
        int num_tasks = q / (2 * s);
        int tasks_per_proc = (num_tasks + size - 1) / size;
        int start_task = rank * tasks_per_proc;
        int end_task = (rank + 1) * tasks_per_proc > num_tasks ? num_tasks : (rank + 1) * tasks_per_proc;

        // Local arrays to store updates
        double *a_new = (double *)malloc((q + 1) * sizeof(double));
        double *b_new = (double *)malloc((q + 1) * sizeof(double));
        double *c_new = (double *)malloc((q + 1) * sizeof(double));
        double *f_new = (double *)malloc((q + 1) * sizeof(double));

        // Copy current coefficients
        for (int j = 0; j <= q; j++) {
            a_new[j] = a[j];
            b_new[j] = b[j];
            c_new[j] = c[j];
            f_new[j] = f[j];
        }

        // Process boundaries (j=0 and j=q)
        if (rank == 0) {
            double beta = -c[0] / b[s];
            b_new[0] = b[0] + beta * a[s];
            f_new[0] = f[0] + beta * f[s];
            c_new[0] = beta * c[s];

            double alpha = -a[q] / b[q - s];
            b_new[q] = b[q] + alpha * c[q - s];
            f_new[q] = f[q] + alpha * f[q - s];
            a_new[q] = alpha * a[q - s];
        }

        // Process inner indices
        for (int task = start_task; task < end_task; task++) {
            int j = (2 * task + 1) * s;
            if (j >= q - s) continue; // Skip invalid indices

            double alpha = -a[j] / b[j - s];
            double beta = -c[j] / b[j + s];
            b_new[j] = b[j] + alpha * c[j - s] + beta * a[j + s];
            f_new[j] = f[j] + alpha * f[j - s] + beta * f[j + s];
            a_new[j] = alpha * a[j - s];
            c_new[j] = beta * c[j + s];
        }

        // Synchronize updates across processes
        MPI_Allreduce(MPI_IN_PLACE, a_new, q + 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        MPI_Allreduce(MPI_IN_PLACE, b_new, q + 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        MPI_Allreduce(MPI_IN_PLACE, c_new, q + 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        MPI_Allreduce(MPI_IN_PLACE, f_new, q + 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

        // Update global arrays
        for (int j = 0; j <= q; j++) {
            a[j] = a_new[j];
            b[j] = b_new[j];
            c[j] = c_new[j];
            f[j] = f_new[j];
        }

        free(a_new);
        free(b_new);
        free(c_new);
        free(f_new);
    }

    // Algorithm 4: Backward Substitution
    // Initial solution for x[0] and x[q]
    if (rank == 0) {
        double denom = b[0] * b[q] - a[0] * c[q];
        x[0] = (f[0] * b[q] - c[q] * f[q]) / denom;
        x[q] = (b[0] * f[q] - a[0] * f[0]) / denom;
    }
    MPI_Bcast(x, q + 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Backward substitution
    for (int s = q / 2; s >= 1; s /= 2) {
        int num_tasks = q / (2 * s);
        int tasks_per_proc = (num_tasks + size - 1) / size;
        int start_task = rank * tasks_per_proc;
        int end_task = (rank + 1) * tasks_per_proc > num_tasks ? num_tasks : (rank + 1) * tasks_per_proc;

        double *x_new = (double *)malloc((q + 1) * sizeof(double));
        for (int j = 0; j <= q; j++) {
            x_new[j] = x[j];
        }

        for (int task = start_task; task < end_task; task++) {
            int j = (2 * task + 1) * s;
            if (j >= q - s) continue;
            x_new[j] = (f[j] - a[j] * x[j - s] - c[j] * x[j + s]) / b[j];
        }

        // Synchronize updates
        MPI_Allreduce(MPI_IN_PLACE, x_new, q + 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

        // Update global x
        for (int j = 0; j <= q; j++) {
            x[j] = x_new[j];
        }

        free(x_new);
    }

    // Print solution (only from rank 0 for clarity)
    if (rank == 0) {
        printf("Solution (first %d elements):\n", N);
        for (int i = 1; i <= N; i++) {
            printf("x[%d] = %f\n", i, x[i]);
        }
    }

    // Clean up
    free(a);
    free(b);
    free(c);
    free(f);
    free(x);

    MPI_Finalize();
    return 0;
}
