#pragma once
#include <string>
#include <deque>
#include "../main/logger.h"

namespace Tests {

enum RecordType {
    RECORD_OPEN,
    RECORD_OPEN_ERROR,
    RECORD_READ,
    RECORD_WRITE,
    RECORD_CLOSE
};

struct OpRecord {
    OpRecord(RecordType _type, pid_t _pid, std::string _path, int _mode, size_t _len, int _error) : type(_type), pid(_pid), path(_path), mode(_mode), len(_len), error(_error) {}

    RecordType type;
    pid_t pid;
    std::string path;
    int mode;
    size_t len;
    int error;
};

class TestLogger : public Logger {
public:
    virtual void open(std::string binpath, pid_t pid, std::string path, int mode, int fd);
    virtual void open_failed(std::string binpath, pid_t pid, std::string path, int mode, int error);
    virtual void read(std::string binpath, pid_t pid, std::string path, int fd, size_t len);
    virtual void write(std::string binpath, pid_t pid, std::string path, int fd, size_t len);
    virtual void close(std::string binpath, pid_t pid, std::string path, int fd);

    void expect_open(pid_t pid, std::string path, int mode);
    void expect_open_failed(pid_t pid, std::string path, int mode, int error);
    void expect_read(pid_t pid, std::string path, size_t len);
    void expect_write(pid_t pid, std::string path, size_t len);
    void expect_close(pid_t pid, std::string path);
    void expect_empty_list();
private:
    bool m_active = false;
    std::deque<OpRecord> m_records;
};

}