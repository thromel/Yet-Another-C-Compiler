int main() {
  int a, b, c[3];
  a = 1 * (2 + 3) % 3;
  b = 1 < 5;
  c[0] = 2;
  c[2] = 69;
  if (a && b)
    c[0]++;
  else
    c[1] = c[0] + c[2];
  a = c[1];
  println(a);
  println(b);
}