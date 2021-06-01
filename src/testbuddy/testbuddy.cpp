#include <iostream>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char** argv) {
    close(-10); //mark the beggining of tests

    int fd = open("/dev/null", O_WRONLY);
    close(fd);

    close(-10); //mark the end of tests
    return 0;
}