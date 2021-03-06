#include <fstream>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>

void call_testhandler(const void* arg) {
    asm("int3" : : "a"(arg));
}

void test_string() {
    call_testhandler("This is a string");
}

void test_string_cap() {
    std::string str(25, 'a');
    str += 'b';
    call_testhandler(str.c_str());
}

void test_string_def() {
    std::string str(PATH_MAX, 'a');
    str += 'b';
    call_testhandler(str.c_str());
}

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

void test_chdir() {
    chdir("dir");
    int fd = open("hello", O_RDWR);
    close(fd);
}

void test_fchdir() {
    DIR* dir = opendir("dir");
    fchdir(dirfd(dir));
    int fd = open("hello", O_RDWR);
    close(fd);
    closedir(dir);
}

void test_rename() {
    rename("hello", "hello2");
}

void test_renameat() {
    renameat(AT_FDCWD, "hello", AT_FDCWD, "hello2");
}

void test_renameat2() {
    renameat2(AT_FDCWD, "hello", AT_FDCWD, "hello2", 0);
}

void test_unlink() {
    unlink("hello");
}

void test_unlinkatcwd() {
    unlinkat(AT_FDCWD, "hello", 0);
}

void test_unlinkat() {
    DIR* dir = opendir("dir");
    unlinkat(dirfd(dir), "hello", 0);
    closedir(dir);
}

void test_rmdir() {
    rmdir("dir");
}

void test_mkdir() {
    mkdir("dir", 0755);
}

void test_mkdiratcwd() {
    mkdirat(AT_FDCWD, "dir", 0755);
}

void test_mkdirat() {
    DIR* dir = opendir("..");
    mkdirat(dirfd(dir), "dir", 0755);
    closedir(dir);
}

void test_forked_chdir(bool is_child) {
    if(is_child)
        chdir("../dir2");
    int fd = open("hello", O_RDWR);
    close(fd);
}

int main(int argc, char** argv) {
    pid_t pid = 0;

    if(argc < 2) {
        std::cerr << "Usage: testbuddy <test name> [fork: 0/1]" << std::endl;
        return 1;
    }

    if(argc >= 3) {
        int fork_opt = atoi(argv[2]);
        if(fork_opt >= 1) {
            if(fork_opt == 2)
                chdir("dir1");
            pid = fork();
            if(pid) {
                int status;
                wait(&status);
            }
            else if(pid < 0) {
                return 1;
            }
        }
    }
    
    close(-10); //mark the beggining of tests

    std::string test(argv[1]);
    if(test == "string")
        test_string();
    else if(test == "string_cap")
        test_string_cap();
    else if(test == "string_def")
        test_string_def();
    else if(test == "simple")
        test_simple();
    else if(test == "relative")
        test_relative();
    else if(test == "openatcwd")
        test_openatcwd();
    else if(test == "openatfd")
        test_openatfd();
    else if(test == "nonexistent")
        test_nonexistent();
    else if(test == "chdir")
        test_chdir();
    else if(test == "fchdir")
        test_fchdir();
    else if(test == "rename")
        test_rename();
    else if(test == "renameat")
        test_renameat();
    else if(test == "renameat2")
        test_renameat2();
    else if(test == "unlink")
        test_unlink();
    else if(test == "unlinkatcwd")
        test_unlinkatcwd();
    else if(test == "unlinkat")
        test_unlinkat();
    else if(test == "rmdir")
        test_rmdir();
    else if(test == "mkdir")
        test_mkdir();
    else if(test == "mkdiratcwd")
        test_mkdiratcwd();
    else if(test == "mkdirat")
        test_mkdirat();
    else if(test == "forked_chdir")
        test_forked_chdir(pid == 0);

    close(-10); //mark the end of tests

    if(pid) {
        std::ofstream pidInfo("/tmp/testpids");
        pidInfo << getpid() << " " << pid;
    }
    
    return 0;
}