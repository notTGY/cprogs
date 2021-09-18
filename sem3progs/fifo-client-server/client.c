#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

#define VERSION 1
#define MAX_STR_LEN 2048
#define BUF_SIZE 1024

int processInput(char* fname) {
  printf("***********************************\n");

  printf("%s", fname);

  printf("***********************************\n");
  return 0;
}

int main() {
  int fd;
  char str[MAX_STR_LEN];
  printf("FIFO client v%d\nEnter your host:", VERSION);
  scanf("%s", str);

  if((fd = open(str, O_WRONLY)) < 0) {
    // error
  }
  while(1) {
    scanf("%s", str);
    processInput(str);
  }
  
  free(str);
  return 0;
}
