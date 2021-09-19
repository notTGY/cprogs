// https://stackoverflow.com/questions/8236/how-do-you-determine-the-size-of-a-file-in-c
off_t fsize(const char *filename) {
  struct stat st;
  if (stat(filename, &st) == 0)
    return st.st_size;

  perror("failed to get file size");
  exit(errno);
}

unsigned int get_line_number(const char* text, int filesize) {
  int i = 0, counter = 0;
  while(i++ < filesize)
    if (text[i] == '\n')
      counter++;
  return counter;
}

int custom_linebreak(int fd) {
  if (write(fd, "--\n", 3) < 0) {
    perror("failed to write to output file");
    exit(errno);
  }
  return 0;
}

int custom_print(int fd, struct str* lines, int line_number) {
  for (int i = 0; i < line_number; i++) {
    if (write(fd, lines[i].text, lines[i].len) < 0) {
      perror("failed to write to output file");
      exit(errno);
    }
  }
  custom_linebreak(fd);
  return 0;
}
