#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "mydev_ioctl.h"

int main(void)
{
    int fd;
    __u32 input = 1234;
    __u32 output = 0;

    fd = open("/dev/mydev", O_RDWR);
    if (fd < 0) {
        perror("open");
        return EXIT_FAILURE;
    }

    printf("Device opened successfully\n");

    if (ioctl(fd, MYDEV_IOC_SET_VALUE, &input) < 0) {
        perror("ioctl SET_VALUE");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("Sent value: %u\n", input);

    if (ioctl(fd, MYDEV_IOC_GET_VALUE, &output) < 0) {
        perror("ioctl GET_VALUE");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("Received value: %u\n", output);

    if (close(fd) < 0) {
        perror("close");
        return EXIT_FAILURE;
    }

    printf("Device closed successfully\n");
    return EXIT_SUCCESS;
}
