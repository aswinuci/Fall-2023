#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int main() {
    const char *filename = "file"; // The name of the file you want to create
    int fd;

    // Create a new file or open an existing one, with read and write permissions (0644)
    fd = open(filename, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if (fd == -1) {
        perror("open");
        return 1;
    } else {
        printf("File '%s' opened with read and write permissions.\n", filename);
    }

    // Check if the file is a regular file
    struct stat file_info;
    if (fstat(fd, &file_info) == -1) {
        perror("fstat");
        close(fd);
        return 1;
    }

    if (S_ISREG(file_info.st_mode)) {
        printf("'%s' is a regular file.\n", filename);
    } else {
        printf("'%s' is not a regular file.\n", filename);
    }

    // Perform any read or write operations here

    // Close the file when done
    close(fd);

    return 0;
}
