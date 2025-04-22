#include<stdio.h>
#include<stdlib.h>

int main() {
  int N, T;
  scanf("%d", &N);
  scanf("%d", &T);
  long *X = (long*)malloc(sizeof(long) * N);
  long *Y = (long*)malloc(sizeof(long) * N);

  for (int i = 0; i < N; i++) {
    scanf("%ld", &(X[i]));
    scanf("%ld", &(Y[i]));
  }

  long *D = (long*)malloc(sizeof(long) * N * N);
  for (int i = 1; i < N; i++) {
    for (int j = 0; j < i; j++) {
      long dx = X[i] - X[j];
      long dy = Y[i] - Y[j];
      long d = dx*dx + dy*dy;
      D[i*N+j] = d;
      D[j*N+i] = d;
    }
  }

  int res = 0;

  for (int i = 2; i < N; i++) {
    for (int j = 1; j < i; j++) {
      for (int k = 0; k < j; k++) {
        long da = D[i*N+j];     
        long db = D[i*N+k];     
        long dc = D[j*N+k];     

        if (T == 2 && da == db && da == dc) {
          res++;
        }
        if (T == 1) {
          if (da == db && dc < 4*da) {
            res++;
          } else if (da == dc && db < 4*da) {
            res++;
          } else if (db == dc && da < 4*db) {
            res++;
          }
        }
      }
    }
  }


  printf("%d", res);

  free(X);
  free(Y);
  free(D);
  return 0;
}
