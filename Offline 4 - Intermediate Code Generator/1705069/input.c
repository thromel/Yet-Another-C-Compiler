int rec(int x) {
  if (x > 0)
    rec(x - 1);
  printf(x);
}

int main() { rec(5); }