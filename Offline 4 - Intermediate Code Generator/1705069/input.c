int rec(int n) {
  if (n == 0)
    return 1;
  return n * rec(n - 1);
}

int main() {
  int x;
  x = rec(7);
  printf(x);
}