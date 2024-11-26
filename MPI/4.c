/*
 * Author: Nikolay Khokhlov <k_h@inbox.ru>, 2016
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <errno.h>

// https://stackoverflow.com/a/26769672
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif

#define ind(i, j) (((i + l->nx) % l->nx) + ((j + l->ny) % l->ny) * (l->nx))

#define DEBUG 0

typedef struct {
	int nx, ny;
	int *u0;
	int *u1;
	int steps;
	int save_steps;

	/* MPI */
	int start, stop;
	int rank;
	int size;
	MPI_Datatype block_type;
	MPI_Datatype comm_block_type;
} life_t;

void life_init(const char *path, life_t *l);
void life_free(life_t *l);
void life_step(life_t *l);
void life_save_vtk(const char *path, life_t *l);
void life_collect(life_t *l);
int life_gather(life_t *l);
void decompisition(const int n, const int p, const int k, int *start, int *stop);

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);
	if (argc != 2) {
		printf("Usage: %s input file.\n", argv[0]);
		return 0;
	}
	life_t l;
	life_init(argv[1], &l);

  struct timespec tms;
  if (clock_gettime(CLOCK_REALTIME, &tms)) {
    perror("Failed to get time");
    MPI_Finalize();
    return 1;
  }
  int64_t start = tms.tv_sec * 1000000;
  start += tms.tv_nsec/1000;
	
	int i;
	char buf[100];
  sprintf(buf, "vtk/life_%06d.vtk", 0);

	for (i = 0; i < l.steps; i++) {
		if (i % l.save_steps == 0) {
      sprintf(buf, "vtk/life_%06d.vtk", i);
      life_collect(&l);
			if (l.rank == 0) {
				printf("Saving step %d to '%s'.\n", i, buf);
				life_save_vtk(buf, &l);
			}
		}
		life_step(&l);
    life_gather(&l);
	}
	
	life_free(&l);

  if (clock_gettime(CLOCK_REALTIME, &tms)) {
    perror("Failed to get time");
    MPI_Finalize();
    return 1;
  }
  int64_t end = tms.tv_sec * 1000000;
  end += tms.tv_nsec/1000;

  if (l.rank == 0) {
    printf("took %ld\n", end - start);
  }

	MPI_Finalize();
	return 0;
}

void life_collect(life_t *l)
{
  //printf("%d collecting\n", l->rank);
  if (l->size == 1) {
    return;
  }
	if (l->rank == 0) {
		int i;
		for(i = 1; i <= l->size - 1; i++) {
			int s1, s2;
			decompisition(l->nx, l->size, i, &s1, &s2);
			MPI_Recv(l->u0 + ind(s1, 0), 1, 
					l->block_type, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	} else {
		MPI_Send(l->u0 + ind(l->start, 0), 1, 
			l->block_type, 0, 0, MPI_COMM_WORLD);
	}
  //printf("%d collected\n", l->rank);
}

int life_gather(life_t *l)
{
  if (l->size == 1) {
    return 0;
  }

	int i1, i2, j;

  int prank = (l->size + l->rank - 1) % l->size;
  int pstop = 0;
  int pstart = 0;
	decompisition(l->nx, l->size, prank, &pstart, &pstop);
  int plen = pstop - pstart;

  int nrank = (l->size + l->rank + 1) % l->size;
  int nstop = 0;
  int nstart = 0;
	decompisition(l->nx, l->size, nrank, &nstart, &nstop);

  if (l->size % 2 && l->rank == l->size - 1) {
    // send to next process
    MPI_Send(l->u0 + ind(l->stop-1, 0), 1, 
      l->comm_block_type, nrank, 0, MPI_COMM_WORLD);
    // block by next process
    MPI_Recv(l->u0 + ind(nstart, 0), 1, 
        l->comm_block_type, nrank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // send to previous process
    MPI_Send(l->u0 + ind(l->start, 0), 1, 
      l->comm_block_type, prank, 0, MPI_COMM_WORLD);
    // receive from previous
    MPI_Recv(l->u0 + ind(pstop-1, 0), 1, 
      l->comm_block_type, prank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
    return 0;
  }

  if (l->rank % 2 == 0) {
    // send to next process
    MPI_Send(l->u0 + ind(l->stop-1, 0), 1, 
      l->comm_block_type, nrank, 0, MPI_COMM_WORLD);
    // block by next process
    MPI_Recv(l->u0 + ind(nstart, 0), 1, 
        l->comm_block_type, nrank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // send to previous process
    MPI_Send(l->u0 + ind(l->start, 0), 1, 
      l->comm_block_type, prank, 0, MPI_COMM_WORLD);
    // receive from previous
    MPI_Recv(l->u0 + ind(pstop-1, 0), 1, 
      l->comm_block_type, prank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  } else {
    // receive from previous
    MPI_Recv(l->u0 + ind(pstop-1, 0), 1, 
      l->comm_block_type, prank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // send to previous process
    MPI_Send(l->u0 + ind(l->start, 0), 1, 
      l->comm_block_type, prank, 0, MPI_COMM_WORLD);
    // block by next process
    MPI_Recv(l->u0 + ind(nstart, 0), 1, 
      l->comm_block_type, nrank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // send to next process
    MPI_Send(l->u0 + ind(l->stop-1, 0), 1, 
      l->comm_block_type, nrank, 0, MPI_COMM_WORLD);
  }
  return 0;

  /*
	if (l->rank == l->size - 1) {
		int i;
		for(i = 0; i < l->size - 1; i++) {
			int s1, s2;
			//decompisition(l->nx, l->size, i, &s1, &s2);
			MPI_Recv(l->u0 + ind(s1, 0), 1, 
					l->block_type, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	} else {
		MPI_Send(l->u0 + ind(l->start, 0), 1, 
			l->block_type, l->size - 1, 0, MPI_COMM_WORLD);
	}
  */
}

/**
 * Загрузить входную конфигурацию.
 * Формат файла, число шагов, как часто сохранять, размер поля, затем идут координаты заполненых клеток:
 * steps
 * save_steps
 * nx ny
 * i1 j2
 * i2 j2
 */
void life_init(const char *path, life_t *l)
{
	FILE *fd = fopen(path, "r");
	assert(fd);
	assert(fscanf(fd, "%d\n", &l->steps));
	assert(fscanf(fd, "%d\n", &l->save_steps));
  if (l->rank == 0) {
    printf("Steps %d, save every %d step.\n", l->steps, l->save_steps);
  }
	assert(fscanf(fd, "%d %d\n", &l->nx, &l->ny));
  if (l->rank == 0) {
    printf("Field size: %dx%d\n", l->nx, l->ny);
  }

	l->u0 = (int*)calloc(l->nx * l->ny, sizeof(int));
	l->u1 = (int*)calloc(l->nx * l->ny, sizeof(int));
	
	int i, j, r, cnt;
	cnt = 0;
	while ((r = fscanf(fd, "%d %d\n", &i, &j)) != EOF) {
		l->u0[ind(i, j)] = 1;
		cnt++;
	}
  if (l->rank == 0) {
    printf("Loaded %d life cells.\n", cnt);
  }
	fclose(fd);


	/* MPI */
	MPI_Comm_size(MPI_COMM_WORLD, &(l->size));
	MPI_Comm_rank(MPI_COMM_WORLD, &(l->rank));
	decompisition(l->nx, l->size, l->rank, &(l->start), &(l->stop));

	//int s1, s2;
	//decompisition(l->nx, l->size, l->rank, &s1, &s2);
	MPI_Type_vector(l->ny, l->stop - l->start, l->nx, MPI_INT, &(l->block_type));
	MPI_Type_commit(&(l->block_type));

	MPI_Type_vector(l->ny, 1, l->nx, MPI_INT, &(l->comm_block_type));
	MPI_Type_commit(&(l->comm_block_type));
}

void life_free(life_t *l)
{
	free(l->u0);
	free(l->u1);
	l->nx = l->ny = 0;
	MPI_Type_free(&(l->block_type));
	MPI_Type_free(&(l->comm_block_type));
}

void life_save_vtk(const char *path, life_t *l)
{
	FILE *f;
	int i1, i2, j;
	f = fopen(path, "w");
	assert(f);
	fprintf(f, "# vtk DataFile Version 3.0\n");
	fprintf(f, "Created by write_to_vtk2d\n");
	fprintf(f, "ASCII\n");
	fprintf(f, "DATASET STRUCTURED_POINTS\n");
	fprintf(f, "DIMENSIONS %d %d 1\n", l->nx+1, l->ny+1);
	fprintf(f, "SPACING %d %d 0.0\n", 1, 1);
	fprintf(f, "ORIGIN %d %d 0.0\n", 0, 0);
	fprintf(f, "CELL_DATA %d\n", l->nx * l->ny);
	
	fprintf(f, "SCALARS life int 1\n");
	fprintf(f, "LOOKUP_TABLE life_table\n");
	for (i2 = 0; i2 < l->ny; i2++) {
		for (i1 = 0; i1 < l->nx; i1++) {
			fprintf(f, "%d\n", l->u0[ind(i1, i2)]);
		}
	}
	fclose(f);
}

void life_step(life_t *l)
{
	int i, j;
	for (j = 0; j < l->ny; j++) {
		for (i = l->start; i < l->stop; i++) {
			int n = 0;
			n += l->u0[ind(i+1, j)];
			n += l->u0[ind(i+1, j+1)];
			n += l->u0[ind(i,   j+1)];
			n += l->u0[ind(i-1, j)];
			n += l->u0[ind(i-1, j-1)];
			n += l->u0[ind(i,   j-1)];
			n += l->u0[ind(i-1, j+1)];
			n += l->u0[ind(i+1, j-1)];
			l->u1[ind(i,j)] = 0;
			if (n == 3 && l->u0[ind(i,j)] == 0) {
				l->u1[ind(i,j)] = 1;
			}
			if ((n == 3 || n == 2) && l->u0[ind(i,j)] == 1) {
				l->u1[ind(i,j)] = 1;
			}
			//l->u1[ind(i,j)] = l->rank;
		}
	}
	int *tmp;
	tmp = l->u0;
	l->u0 = l->u1;
	l->u1 = tmp;
}

void decompisition(const int n, const int p, const int k, int *start, int *stop)
{
	int l = n / p; // длинна куска
	*start = l * k;
	*stop = *start + l;
	if (k == p - 1) *stop = n;
}
