#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

int main() {
    // Virtual address where you want the heap to start
    uintptr_t VIRT_ADDR = 0x600000000000;

    // File size in bytes
    size_t file_size = 4096 * 100000;  // 4 KB * 100000 = 400 MB

    // Calculate the program break based on VIRT_ADDR and file size
    void *desired_program_break = (void *)(VIRT_ADDR + file_size);

    // Set the program break to the desired location
    if (sbrk(desired_program_break - sbrk(0)) == (void *)-1) {
        perror("sbrk");
        return 1;
    }

    // Now, the program break is set to allocate memory only within the file size
    printf("Program break set to: %p\n", sbrk(0));

    // You can allocate memory within this specified range

    return 0;
}
