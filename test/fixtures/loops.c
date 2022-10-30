int main() {
    int sum = 0;
    int i = 0;
    
    while (i < 10) {
        sum = sum + i;
        i = i + 1;
    }
    
    for (int j = 0; j < 5; j++) {
        sum = sum + j;
    }
    
    return sum;
}
