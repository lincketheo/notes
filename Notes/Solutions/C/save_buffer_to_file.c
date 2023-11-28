#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

int save_bytes(const void *input, const size_t length, const char* filename_format, ...) {
    va_list args;
    va_start(args, filename_format);
    char filename[256];
    vsnprintf(filename, sizeof(filename), filename_format, args);

    FILE *fid = fopen(filename, "wb");
    if(fid == NULL) {
        perror("Error opening file");
        return -1;
    }

    size_t bytes = fwrite(input, 1, length, fid);
    if(bytes != length){
        perror("Error writing to file");
        fclose(fid);
        return -1;
    }

    fclose(fid);
    return 0;
}

int main() {
    int data[5] = {1, 2, 3, 4, 5};
    save_bytes(data, 5 * sizeof(int), "file_%d.bin", 0);
    return 0;
}