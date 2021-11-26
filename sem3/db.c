#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/mman.h>
#include<errno.h>

#define SIZE 512

int main(int argc, char** argv) {
  int fd = open("./db.txt", O_RDWR);
  char* ptr;
  int iteration = 0;
  int line = 0;
  int found = 0;
  
  if (argc < 3) {
    printf("usage %s <ID> <Word>\n", argv[0]);
    return 0;
  }

printf("0\n");
  int id = atoi(argv[1]);

printf("1: %d\n", id);
  while(!found) {
printf("2\n");
    ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, iteration*SIZE);
    if (ptr == MAP_FAILED) {
      perror("failed to mmap file");
      exit(errno);
    }
    for(int i = 0; i < SIZE; i++) {
printf("3: %d\n", i);
      if (!ptr[i]) {
        ftruncate(fd, (iteration + 1) * SIZE);
printf("4\n");
        while (line < id) {
          ptr[i++] = '\n';
          int j = 0;
          while (argv[1][j]) ptr[i++] = argv[1][j++];
          line++;
        }
        found = i;
      }
      if (ptr[i] == '\n') line++;
      if (line == id) {
        found = i;
      }
    }
printf("5\n");
    munmap(ptr, SIZE);
  }

  return 0;
}
