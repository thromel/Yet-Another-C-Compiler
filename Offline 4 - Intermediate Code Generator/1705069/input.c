int sumf(int a, int b) {
  int t;
  if (!a)
    return 0;
  return (a + b) + sumf(a - 1, b - 1);
}

int main() {
  int x;
  x = sumf(10, 11);
  println(x);
}