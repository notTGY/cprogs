#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>

#define VERSION 1
#define MAX_STR_LEN 2048
#define BUF_SIZE 1024

int processInput(char* fname, int write_d, int read_d, char* read_fifo) {
  char buf[BUF_SIZE];
  sprintf(buf, "%s %s", read_fifo, fname)
  memcpy(buf, read_d);
  fwrite(write_d, buf, BUF_SIZE);
  printf("*************************\n%s\n*************************\n", fname);
  return 0;
}

int main() {
  int fd_write, fd_read
  char str[MAX_STR_LEN]
  char *fifo = "hi";
  printf("FIFO client v%d\nEnter your host:", VERSION);
  scanf("%s", str);

  if((fd_write = open(str, O_WRONLY)) < 0) {
    // error
  }

  if (access(fifo, F_OK) == 0) {
    // file exists
  } else {
    // file does not exist
  }

  while(1) {
    scanf("%s", str);
    processInput(str, fd_write, fd_read, fifo);
  }
  
  free(str);
  return 0;
}
