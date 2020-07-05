int main() {
  int c[21], i, b;
  c[0] = 0;
  c[1] = 1;

  for (i = 2; i < 21; i++) {
    c[i] = c[i - 1] + c[i - 2];
    b = c[i - 1] + c[i - 2];
    printf(b);
  }

  for (i = 0; i < 21; i++) {
    b = c[i];
    printf(b);
  }
}