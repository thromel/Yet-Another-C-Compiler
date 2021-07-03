int main() {
  int a[20], i, b;
  for (i = 0; i < 20; i++) {
    a[i] = i + 1;
  }
  for (i = 0; i < 20; i++) {
    b = a[i];
    printf(b);
  }
}