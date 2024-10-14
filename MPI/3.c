/*
 * Author: Nikolay Khokhlov <k_h@inbox.ru>, 2016
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <mpi.h>

#define ind(i, j) (((i + l->nx) % l->nx) + ((j + l->ny) % l->ny) * (l->nx))

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
} life_t;

void life_init(const char *path, life_t *l);
void life_free(life_t *l);
void life_step(life_t *l);
void life_save_vtk(const char *path, life_t *l, int should_save);
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
	
	int i;
	char buf[100];
	for (i = 0; i < l.steps; i++) {
		if (i % l.save_steps == 0) {
			sprintf(buf, "vtk/life_%06d.vtk", i);
			// printf("Saving step %d to '%s'.\n", i, buf);
		}
    life_save_vtk(buf, &l, i % l.save_steps == 0);
		life_step(&l);
	}
	
	life_free(&l);
	MPI_Finalize();
	return 0;
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
	/* MPI */
	MPI_Comm_size(MPI_COMM_WORLD, &(l->size));
	MPI_Comm_rank(MPI_COMM_WORLD, &(l->rank));

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

	decompisition(l->ny, l->size, l->rank, &(l->start), &(l->stop));

  printf("stop: %d; start: %d\n", l->stop, l->start);

  int len = l->stop - l->start;


	l->u0 = (int*)calloc(l->nx * l->ny, sizeof(int));
	l->u1 = (int*)calloc(l->nx * l->ny, sizeof(int));
	
	int i, j, r, cnt;
	cnt = 0;
	while ((r = fscanf(fd, "%d %d\n", &i, &j)) != EOF) {
    int k = ind(i, j);
    l->u0[k] = 1;
    cnt++;
	}
	printf("%d Loaded %d life cells.\n", l->rank, cnt);
	fclose(fd);

}

void life_free(life_t *l)
{
	free(l->u0);
	free(l->u1);
	l->nx = l->ny = 0;
}

void life_save_vtk(const char *path, life_t *l, int should_save)
{
	FILE *f;
	int i1, i2, j;
  int len = l->stop - l->start;

  int prank = (l->size + l->rank - 1) % l->size;
  int pstop = 0;
  int pstart = 0;
	decompisition(l->ny, l->size, prank, &pstart, &pstop);
  int plen = pstop - pstart;

  int nrank = (l->size + l->rank + 1) % l->size;
  int nstop = 0;
  int nstart = 0;
	decompisition(l->ny, l->size, nrank, &nstart, &nstop);
  int nlen = nstop - nstart;

  if (l->rank == 0 && should_save) {
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
  }
  
  int buf[100];
  if (l->rank > 0) {
    MPI_Recv(
      &l->u0[ind(0, pstart)],
      l->nx * plen,
      MPI_INT,
      prank,
      0,
      MPI_COMM_WORLD,
      MPI_STATUS_IGNORE
    );
    if (should_save) {
      f = fopen(path, "a");
      assert(f);
    }
  }

  if (should_save) {
    for (i2 = l->start; i2 < l->stop; i2++) {
      for (i1 = 0; i1 < l->nx; i1++) {
        fprintf(f, "%d\n", l->u0[ind(i1, i2)]);
      }
    }
    fclose(f);
  }

  if (l->size > 1) {
    // send to next process
    MPI_Send(
      &l->u0[ind(0, l->start)],
      l->nx * len,
      MPI_INT,
      nrank,
      0,
      MPI_COMM_WORLD
    );
    // send to previous process
    MPI_Send(
      &l->u0[ind(0, l->start)],
      l->nx * len,
      MPI_INT,
      prank,
      0,
      MPI_COMM_WORLD
    );

    // block by next process
    MPI_Recv(
      &l->u0[ind(0, nstart)],
      l->nx * nlen,
      MPI_INT,
      nrank,
      0,
      MPI_COMM_WORLD,
      MPI_STATUS_IGNORE
    );

    // additionally block first process till last is done
    if (l->rank == 0) {
      MPI_Recv(
        &l->u0[ind(0, pstart)],
        l->nx * plen,
        MPI_INT,
        prank,
        0,
        MPI_COMM_WORLD,
        MPI_STATUS_IGNORE
      );
    }
  }
}

void life_step(life_t *l)
{
	int i, j;
	for (j = l->start; j < l->stop; j++) {
		for (i = 0; i < l->nx; i++) {
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
	if (k == n - 1) *stop = n;
}
