#include <iostream>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

void test_simple() {
    int fd = open("/dev/null", O_WRONLY);
    close(fd);
}

void test_relative() {
    int fd = open("../../hello", O_RDWR);
    close(fd);
}

void test_openatcwd() {
    int fd = openat(AT_FDCWD, "hello", O_RDWR);
    close(fd);
}

void test_openatfd() {
    DIR* dir = opendir("..");
    int fd = openat(dirfd(dir), "hello", O_RDWR);
    close(fd);
    closedir(dir);
}

void test_nonexistent() {
    open("doesntexist", O_RDONLY);
}

int main(int argc, char** argv) {
    if(argc < 2) {
        std::cerr << "Usage: testbuddy <test name>" << std::endl;
        return 1;
    }
    
    close(-10); //mark the beggining of tests

    std::string test(argv[1]);
    if(test == "simple")
        test_simple();
    else if(test == "relative")
        test_relative();
    else if(test == "openatcwd")
        test_openatcwd();
    else if(test == "openatfd")
        test_openatfd();
    else if(test == "nonexistent")
        test_nonexistent();

    close(-10); //mark the end of tests
    return 0;
}