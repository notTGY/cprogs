#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<errno.h>

// https://stackoverflow.com/questions/8236/how-do-you-determine-the-size-of-a-file-in-c
off_t fsize(const char *filename) {
  struct stat st;
  if (stat(filename, &st) == 0)
    return st.st_size;

  perror("failed to get file size");
  exit(errno);
}

/**
 * function gets text and returns number of newlines
 */
unsigned int get_line_number(const char* text, int filesize) {
  int i = 0, counter = 0;
  while(i++ < filesize)
    if (text[i] == '\n')
      counter++;
  return counter;
}

/**
 * split text between lines without modifying the text itself
 */
int process_line_splitting(char* text, char** lines, int* line_lenghts, int filesize) {
  int i = 0, line = 0;

  lines[0] = &text[0];
  line_lenghts[0] = 0;

  while(i++ < filesize) {
    if (text[i] == '\n')
      lines[++line] = &text[i];
    line_lenghts[line]++;
  }
}

/**
 * function to custom print our lines
 */
int custom_print(int fd, char** lines, int* line_lenghts, int line_number) {
  for (int i = 0; i < line_number; i++) {
    write(fd, lines[i], line_lenghts[i]);
  }
  return 0;
}

int main(int argc, char** argv) {
  char *filename, *buffer, **lines;
  off_t filesize;
  int fd, line_number;
  int *line_lenghts;
  if (argc == 1) {
    printf("usage: %s <filename>\n", argv[0]);
    return 0;
  }
  filename = argv[1];
  filesize = fsize(filename);
  buffer = malloc((filesize+1) * sizeof(char));
  if (buffer == NULL) {
    perror("failed to allocate memory");
    exit(errno);
  }
  
  if ((fd = open(filename, O_RDONLY)) < 0) {
    perror("failed to open file");
    exit(errno);
  }
  // O(n)
  if (read(fd, buffer, filesize) < 0) {
    perror("failed to read file");
    exit(errno);
  }

  // O(n)
  line_number = get_line_number(buffer, filesize);
  lines = malloc((line_number + 1) * sizeof(char*));
  line_lenghts = malloc((line_number + 1) * sizeof(int));
  for (int i = 0; i < line_number + 1; i++)
    line_lenghts[i] = 0;

  // O(n)
  process_line_splitting(buffer, lines, line_lenghts, filesize);

  custom_print(1, lines, line_lenghts, line_number);

  free(lines);
  free(line_lenghts);
  free(buffer);
  return 0;
}
