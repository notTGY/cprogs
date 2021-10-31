#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<pthread.h>

struct thread_in { int row; int col; };

unsigned long *thid;
struct thread_in *pdata;
int *mat1;
int *mat2;
int *res;
int n;
int size;

void* threader(void *data) {
	int row = ((struct thread_in*)data)->row;
	int col = ((struct thread_in*)data)->col;

  int r = 0;
  for (int i = 0; i < n; i++) {
    r += mat1[n * row + i] * mat2[n * col + i];
  }
  res[row * n + col] = r;

	pthread_exit(NULL);
}

int main(int argc, char** argv) {
  if (argc < 2) {
    printf("usage %s <filename>\n", argv[0]);
    return 1;
  }
  if (freopen(argv[1], "rt", stdin) == NULL) {
    perror("failed to open file");
    exit(errno);
  }

  scanf("%d", &n);
  size = n * n;
  mat1 = malloc(sizeof(int) * size);
  mat2 = malloc(sizeof(int) * size);
  res = malloc(sizeof(int) * size);
  pdata = malloc(sizeof(struct thread_in) * size);
  thid = malloc(sizeof(unsigned int) * size);

  for (int i = 0; i < size; i++) {
    scanf("%d", &(mat1[i]));
  }
  for (int i = 0; i < size; i++) {
    int col = i % n;
    int row = (i - col) / n;
    scanf("%d", &(mat2[col * n + row]));
  }

	for (int i = 0; i < size; i++) {
    pdata[i].row = i % n;
    pdata[i].col = i / n;
		if (pthread_create(&thid[i], NULL, threader, (void*)&pdata[i]) != 0) {
			perror("failed to create thread");
			exit(errno);
		}
	}
	for (int i = 0; i < size; i++) {
		if (pthread_join(thid[i], NULL) != 0) {
			perror("failed to join thread");
			exit(errno);
		}
	}

  printf("\n");
  for (int i = 0; i < size; i++) {
    printf("%d ", res[i]);
    if (i % n == n - 1) printf("\n");
  }
  free(thid);
  free(pdata);
  free(mat1);
  free(mat2);
  free(res);

	return 0;
}
