int main() {
    int x = "hello";  // Type error: assigning string to int
    float y = x + y;  // Error: y used before being defined
    return x && "test";  // Error: logical AND with string
}
