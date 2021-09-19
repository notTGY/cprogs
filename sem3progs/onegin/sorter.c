#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<errno.h>

struct str {
  char *text;
  int len;
};

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
int process_line_splitting(char* text, struct str* lines, int filesize) {
  int i = 0, line = 0;

  lines[0].text = &text[0];
  lines[0].len = 0;

  while(i++ < filesize) {
    if (text[i] == '\n')
      lines[++line].text = &text[i];
    lines[line].len++;
  }
}

/**
 * function to custom print our lines
 */
int custom_print(int fd, struct str* lines, int line_number) {
  for (int i = 0; i < line_number; i++) {
    write(fd, lines[i].text, lines[i].len);
  }
  return 0;
}

/**
 * comparator function that sorts lines alphabetically
 */
int strcmpup(const void *a1, const void *a2) {
  const struct str *p1 = (const struct str*)a1;
  const struct str *p2 = (const struct str*)a2;
  int min_len = p1->len < p2->len ? p1->len : p2->len;
  for (int i = 0; i < min_len; i++) {
    if (p1->text[i] < p2->text[i]) return -1;
    if (p1->text[i] > p2->text[i]) return 1;
  }
  if (p1->len < p2->len) return -1;
  if (p1->len > p2->len) return 1;
  return 0;
}

/**
 * comparator function that sorts lines reversed alphabetically
 */
int strcmpdown(const void *a1, const void *a2) {
  const struct str *p1 = (const struct str*)a1;
  const struct str *p2 = (const struct str*)a2;
  int min_len = p1->len < p2->len ? p1->len : p2->len;
  for (int i = 0; i < min_len; i++) {
    if (p1->text[p1->len - i] < p2->text[p2->len - i]) return -1;
    if (p1->text[p1->len - i] > p2->text[p2->len - i]) return 1;
  }
  if (p1->len < p2->len) return -1;
  if (p1->len > p2->len) return 1;
  return 0;
}


int main(int argc, char** argv) {
  char *filename, *buffer, *output;
  struct str *lines;
  off_t filesize;
  int fd, line_number, fd_output;
  if (argc != 3) {
    printf("usage: %s <input filename> <output filename>\n", argv[0]);
    return 0;
  }
  filename = argv[1];
  output = argv[2];
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
  if ((fd_output = open(output, O_WRONLY)) < 0) {
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
  lines = malloc((line_number + 1) * sizeof(struct str));
  for (int i = 0; i < line_number + 1; i++)
    lines[i].len = 0;

  // O(n)
  process_line_splitting(buffer, lines, filesize);

  qsort(lines, line_number + 1, sizeof(struct str), strcmpup);
  custom_print(fd_output, lines, line_number);
  write(fd_output, "\n\n\n", 3);

  qsort(lines, line_number + 1, sizeof(struct str), strcmpdown);
  custom_print(fd_output, lines, line_number);
  write(fd_output, "\n\n\n", 3);

  write(fd_output, buffer, filesize);

  free(lines);
  free(buffer);
  return 0;
}
