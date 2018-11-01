#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
int main() {
    FILE *f = tmpfile();
    int *m = mmap(0, 4, PROT_WRITE, MAP_PRIVATE, fileno(f), 0);
    *m = 0;
    return 0;
}
