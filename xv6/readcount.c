#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int initial_count, final_count, fd;
    char buffer[100];
    
    // Get initial read count
    initial_count = getreadcount();
    printf("Initial read count: %d\n", initial_count);
    
    // Create a test file if it doesn't exist
    fd = open("testfile.txt", O_CREATE | O_WRONLY);
    if (fd >= 0) {
        write(fd, "This is a test file with more than 100 characters to ensure we can read 100 bytes from it successfully. This should be enough content for our test case.", 150);
        close(fd);
    }
    
    // Open file for reading
    fd = open("testfile.txt", O_RDONLY);
    if (fd < 0) {
        printf("Failed to open test file\n");
        exit(1);
    }
    
    // Read 100 bytes
    int bytes_read = read(fd, buffer, 100);
    close(fd);
    
    if (bytes_read < 0) {
        printf("Read failed\n");
        exit(1);
    }
    
    printf("Successfully read %d bytes\n", bytes_read);
    
    // Get final read count
    final_count = getreadcount();
    printf("Final read count: %d\n", final_count);
    
    // Verify the increase
    int increase = final_count - initial_count;
    printf("Increase in read count: %d\n", increase);
    
    if (increase >= bytes_read) {
        printf("✓ Read count increased correctly!\n");
    } else {
        printf("✗ Read count did not increase as expected\n");
    }
    
    exit(0);
}
