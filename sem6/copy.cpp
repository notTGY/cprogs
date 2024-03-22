template <typename T, typename G>
void copy_array(T from[], G to[], int size = 1) {
  for (int i = 0; i < size; i++) {
    to[i] = from[i];
  }
}