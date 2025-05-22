#include <stdio.h>
#include "mpi.h"
#include <math.h>
#include <stdlib.h>

#define FLOAT double
#define FLOAT_MPI MPI_DOUBLE
#define MAX_n_bar 2048
#define MAX MAX_n_bar*MAX_n_bar

typedef struct {
    int p;
    MPI_Comm comm, row_comm, col_comm;
    int q, my_row, my_col, my_rank;
} GRID_INFO_T;

typedef struct {
    int n_bar;
    #define Order(A) ((A)->n_bar)
    FLOAT entries[MAX];
    #define Entry(A,i,j) (*(((A)->entries) + ((A)->n_bar)*(i) + (j)))
} LOCAL_MATRIX_T;

MPI_Datatype local_matrix_mpi_t;

LOCAL_MATRIX_T* Local_matrix_allocate(int n_bar) {
    LOCAL_MATRIX_T* matrix = (LOCAL_MATRIX_T*) malloc(sizeof(LOCAL_MATRIX_T));
    //matrix->entries = (FLOAT*) malloc(sizeof(FLOAT) * n_bar*n_bar);
    return matrix;
}

void Free_local_matrix(LOCAL_MATRIX_T** local_A) {
    //free((*local_A)->entries);
    free(*local_A);
}

void Setup_grid(GRID_INFO_T* grid) {
    int dimensions[2], wrap_around[2] = {1, 1}, coordinates[2], free_coords[2];
    MPI_Comm_size(MPI_COMM_WORLD, &grid->p);
    grid->q = (int) sqrt((double) grid->p);
    dimensions[0] = dimensions[1] = grid->q;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dimensions, wrap_around, 1, &grid->comm);
    MPI_Comm_rank(grid->comm, &grid->my_rank);
    MPI_Cart_coords(grid->comm, grid->my_rank, 2, coordinates);
    grid->my_row = coordinates[0];
    grid->my_col = coordinates[1];
    free_coords[0] = 0; free_coords[1] = 1;
    MPI_Cart_sub(grid->comm, free_coords, &grid->row_comm);
    free_coords[0] = 1; free_coords[1] = 0;
    MPI_Cart_sub(grid->comm, free_coords, &grid->col_comm);
}

void Build_matrix_type(LOCAL_MATRIX_T* local_A) {
    MPI_Datatype temp_mpi_t;
    int block_lengths[2] = {1, 1};
    MPI_Aint displacements[2];
    MPI_Datatype typelist[2] = {MPI_INT, MPI_DATATYPE_NULL};
    MPI_Aint start_address, address;
    MPI_Type_contiguous(Order(local_A) * Order(local_A), FLOAT_MPI, &temp_mpi_t);
    typelist[1] = temp_mpi_t;
    MPI_Get_address(local_A, &start_address);
    MPI_Get_address(&local_A->n_bar, &address);
    displacements[0] = address - start_address;
    MPI_Get_address(local_A->entries, &address);
    displacements[1] = address - start_address;
    MPI_Type_create_struct(2, block_lengths, displacements, typelist, &local_matrix_mpi_t);
    MPI_Type_commit(&local_matrix_mpi_t);
}

void Set_to_zero(LOCAL_MATRIX_T* local_A) {
    for (int i = 0; i < Order(local_A); i++)
        for (int j = 0; j < Order(local_A); j++)
            Entry(local_A, i, j) = 0.0;
}

void Local_matrix_multiply(LOCAL_MATRIX_T* local_A, LOCAL_MATRIX_T* local_B, LOCAL_MATRIX_T* local_C) {
    for (int i = 0; i < Order(local_A); i++)
        for (int j = 0; j < Order(local_A); j++)
            for (int k = 0; k < Order(local_B); k++)
                Entry(local_C, i, j) += Entry(local_A, i, k) * Entry(local_B, k, j);
}

void Fox(int n, GRID_INFO_T* grid, LOCAL_MATRIX_T* local_A, LOCAL_MATRIX_T* local_B, LOCAL_MATRIX_T* local_C) {
    LOCAL_MATRIX_T* temp_A = Local_matrix_allocate(n / grid->q);
    int source = (grid->my_row + 1) % grid->q;
    int dest = (grid->my_row + grid->q - 1) % grid->q;
    MPI_Status status;
    Set_to_zero(local_C);
    for (int stage = 0; stage < grid->q; stage++) {
        int bcast_root = (grid->my_row + stage) % grid->q;
        if (bcast_root == grid->my_col) {
            MPI_Bcast(local_A, 1, local_matrix_mpi_t, bcast_root, grid->row_comm);
            Local_matrix_multiply(local_A, local_B, local_C);
        } else {
            MPI_Bcast(temp_A, 1, local_matrix_mpi_t, bcast_root, grid->row_comm);
            Local_matrix_multiply(temp_A, local_B, local_C);
        }
        if (stage != grid->q - 1) {
          MPI_Sendrecv_replace(local_B, 1, local_matrix_mpi_t, dest, 0, source, 0, grid->col_comm, &status);
        }
    }
    Free_local_matrix(&temp_A);
}

void Print_matrix(char* title, LOCAL_MATRIX_T* local_A, GRID_INFO_T* grid, int n) {
    FLOAT* temp = (FLOAT*) malloc(Order(local_A) * sizeof(FLOAT));
    MPI_Status status;
    if (grid->my_rank == 0) {
        printf("%s\n", title);
        for (int mat_row = 0; mat_row < n; mat_row++) {
            int grid_row = mat_row / Order(local_A);
            int coords[2] = {grid_row, 0};
            for (int grid_col = 0; grid_col < grid->q; grid_col++) {
                coords[1] = grid_col;
                int source;
                MPI_Cart_rank(grid->comm, coords, &source);
                if (source == 0) {
                    for (int mat_col = 0; mat_col < Order(local_A); mat_col++)
                        printf("%10.5f ", Entry(local_A, mat_row % Order(local_A), mat_col));
                } else {
                    MPI_Recv(temp, Order(local_A), FLOAT_MPI, source, 0, grid->comm, &status);
                    for (int mat_col = 0; mat_col < Order(local_A); mat_col++)
                        printf("%10.5f ", temp[mat_col]);
                }
            }
            printf("\n");
        }
        free(temp);
    } else {
        for (int mat_row = 0; mat_row < Order(local_A); mat_row++)
            MPI_Send(&Entry(local_A, mat_row, 0), Order(local_A), FLOAT_MPI, 0, 0, grid->comm);
    }
}

int main(int argc, char* argv[]) {
    int size, my_rank, n, n_bar, matrix_max;
    GRID_INFO_T grid;
    LOCAL_MATRIX_T *local_A, *local_B, *local_C;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    Setup_grid(&grid);

    if (my_rank == 0) {
      if (argc > 1) {
        n = atoi(argv[1]);
        if (n <= 0) {
          printf("Error: n must be a positive integer\n");
          MPI_Finalize();
          return 1;
        }
        if (n % size != 0) {
          printf("Error: n must be a multiple of %d\n", size);
          MPI_Finalize();
          return 1;
        }
      } else {
        n = 8;
      }
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    n_bar = n / grid.q;
    matrix_max = n_bar * n_bar;
    if (matrix_max > MAX) {
      printf("Error: %d > %d\n", matrix_max, MAX);
      MPI_Finalize();
      return 1;
    }

    local_A = Local_matrix_allocate(n_bar);
    Order(local_A) = n_bar;
    local_B = Local_matrix_allocate(n_bar);
    Order(local_B) = n_bar;
    local_C = Local_matrix_allocate(n_bar);
    Order(local_C) = n_bar;

    for (int i = 0; i < n_bar; i++) {
        for (int j = 0; j < n_bar; j++) {
            Entry(local_A, i, j) = 1.0;
            Entry(local_B, i, j) = 1.0;
        }
    }

    Build_matrix_type(local_A);
    Fox(n, &grid, local_A, local_B, local_C);
    //Print_matrix("The product is", local_C, &grid, n);

    Free_local_matrix(&local_A);
    Free_local_matrix(&local_B);
    Free_local_matrix(&local_C);
    MPI_Finalize();
    return 0;
}
