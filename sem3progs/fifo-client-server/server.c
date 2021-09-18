#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define DEFAULT_HOST_NAME "./fifo-server"
#define BUF_SIZE 1024

int main(int argc, char** argv) {
  char fifo_name[BUF_SIZE], filename[BUF_SIZE], buf[BUF_SIZE];
  int fd;
  if (argc < 2) {
    printf("fifo host not stated - defaults to '%s'", DEFAULT_HOST_NAME);
    umask(0);
    if(mkfifo(DEFAULT_HOST_NAME, 0600) < 0) {
      perror("failed to make fifo");
      exit(errno);
    }
    fd = open(DEFAULT_HOST_NAME, O_RDONLY);
  } else {
    umask(0);
    if(mkfifo(argv[1], 0600) < 0) {
      perror("failed to make fifo");
      exit(errno);
    }
    fd = open(argv[1], O_RDONLY);
  }
  while(1) {
    read(fd, buf, BUF_SIZE);
    sscanf("%s %s", fifo_name, filename);
  }

  return 0;
}
