#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<errno.h>
#include<ctype.h>

#include"types.c"
#include"utils.c"

/**
 * split text between lines without modifying the text itself
 */
int process_line_splitting(char* text, struct str* lines, int filesize) {
  int i = 0, line = 0;

  lines[0].text = &text[0];
  lines[0].len++;

  while(++i < filesize) {
    lines[line].len++;
    if (text[i] == '\n')
      lines[++line].text = &text[i+1];
  }
}

int strcmpup(const void *a1, const void *a2) {
  const struct str *s1 = (const struct str*)a1;
  const struct str *s2 = (const struct str*)a2;

  int min_len = s1->len < s2->len ? s1->len : s2->len;

  for (int i = 0; i < min_len; i++) {
    char c1 = tolower(s1->text[i]);
    char c2 = tolower(s2->text[i]);
    if (c1 < c2) return -1;
    if (c1 > c2) return 1;
  }

  if (s1->len < s2->len) return -1;
  if (s1->len > s2->len) return 1;
  return 0;
}

int strcmpdown(const void *a1, const void *a2) {
  const struct str *s1 = (const struct str*)a1;
  const struct str *s2 = (const struct str*)a2;

  int min_len = s1->len < s2->len ? s1->len : s2->len;

  for (int i = 1; i < min_len + 1; i++) {
    int i1 = s1->len - i;
    int i2 = s2->len - i;
    char c1 = tolower(s1->text[i1]);
    char c2 = tolower(s2->text[i2]);
    if (c1 < c2) return -1;
    if (c1 > c2) return 1;
  }

  if (s1->len < s2->len) return -1;
  if (s1->len > s2->len) return 1;
  return 0;
}


int main(int argc, char** argv) {
  char *filename_in, *buffer, *filename_out;
  struct str *lines;
  off_t filesize;
  int fd_in, line_number, fd_out;
  if (argc != 3) {
    printf("usage: %s <input filename> <output filename>\n", argv[0]);
    return 0;
  }
  filename_in = argv[1];
  filename_out = argv[2];
  filesize = fsize(filename_in);
  buffer = malloc((filesize+1) * sizeof(char));
  if (buffer == NULL) {
    perror("failed to allocate memory");
    exit(errno);
  }
  
  if ((fd_in = open(filename_in, O_RDONLY)) < 0) {
    perror("failed to open input file");
    exit(errno);
  }
  if ((fd_out = open(filename_out, O_WRONLY)) < 0) {
    perror("failed to open output file");
    exit(errno);
  }
  if (read(fd_in, buffer, filesize) < 0) {
    perror("failed to read input file");
    exit(errno);
  }

  line_number = get_line_number(buffer, filesize) + 1;
  lines = malloc(line_number * sizeof(struct str));
  if (lines == NULL) {
    perror("failed to allocate memory");
    exit(errno);
  }
  for (int i = 0; i < line_number; i++)
    lines[i].len = 0;

  process_line_splitting(buffer, lines, filesize);

  qsort(lines, line_number, sizeof(struct str), strcmpup);
  custom_print(fd_out, lines, line_number);

  qsort(lines, line_number, sizeof(struct str), strcmpdown);
  custom_print(fd_out, lines, line_number);

  if (write(fd_out, buffer, filesize) < 0) {
    perror("failed to write to output file");
    exit(errno);
  }
  custom_linebreak(fd_out);

  free(lines);
  free(buffer);
  return 0;
}
