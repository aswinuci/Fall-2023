#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int main() {
    const char *filename = "file"; // The name of the file you created with dd

    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    struct stat file_info;
    if (fstat(fd, &file_info) == -1) {
        perror("fstat");
        return 1;
    }

    printf("File size: %ld bytes\n", file_info.st_size);
    // You can access other file attributes using the struct stat members.

    close(fd);
    return 0;
}
