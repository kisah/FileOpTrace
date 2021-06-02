#include "testlogger.h"
#include <iostream>

using namespace Tests;

void TestLogger::open(std::string binpath, pid_t pid, std::string path, int mode, int fd) {
    if(m_active)
        m_records.emplace_back(OpRecord(RECORD_OPEN, pid, path, mode, 0, 0));
}

void TestLogger::open_failed(std::string binpath, pid_t pid, std::string path, int mode, int error) {
    if(m_active)
        m_records.emplace_back(OpRecord(RECORD_OPEN_ERROR, pid, path, mode, 0, error));
}

void TestLogger::read(std::string binpath, pid_t pid, std::string path, int fd, size_t len) {
    if(m_active)
        m_records.emplace_back(OpRecord(RECORD_READ, pid, path, 0, len, 0));
}

void TestLogger::write(std::string binpath, pid_t pid, std::string path, int fd, size_t len) {
    if(m_active)
        m_records.emplace_back(OpRecord(RECORD_WRITE, pid, path, 0, len, 0));
}

void TestLogger::close(std::string binpath, pid_t pid, std::string path, int fd) {
    if(fd == -10)
        m_active = !m_active;
    else if(m_active)
        m_records.emplace_back(OpRecord(RECORD_CLOSE, pid, path, 0, 0, 0));
}

void TestLogger::expect_open(pid_t pid, std::string path, int mode) {
    if(!m_records.size())
        throw "Record list is empty";
    
    auto record = m_records.front();
    m_records.pop_front();

    if(record.type != RECORD_OPEN)
        throw "Record type doesn't match";

    if(record.pid != pid && pid != -1)
        throw "Process id doesn't match";

    if(record.path != path)
        throw "Path doesn't match";

    if(record.mode != mode)
        throw "Open mode doesn't match";
}

void TestLogger::expect_open_failed(pid_t pid, std::string path, int mode, int error) {
    if(!m_records.size())
        throw "Record list is empty";
    
    auto record = m_records.front();
    m_records.pop_front();

    if(record.type != RECORD_OPEN_ERROR)
        throw "Record type doesn't match";

    if(record.pid != pid && pid != -1)
        throw "Process id doesn't match";

    if(record.path != path)
        throw "Path doesn't match";

    if(record.mode != mode)
        throw "Open mode doesn't match";

    if(record.error != error)
        throw "Unexpected error condition";
}

void TestLogger::expect_read(pid_t pid, std::string path, size_t len) {
    if(!m_records.size())
        throw "Record list is empty";
    
    auto record = m_records.front();
    m_records.pop_front();

    if(record.type != RECORD_READ)
        throw "Record type doesn't match";

    if(record.pid != pid && pid != -1)
        throw "Process id doesn't match";

    if(record.path != path)
        throw "Path doesn't match";

    if(record.len != len)
        throw "Read length doesn't match";
}

void TestLogger::expect_write(pid_t pid, std::string path, size_t len) {
    if(!m_records.size())
        throw "Record list is empty";
    
    auto record = m_records.front();
    m_records.pop_front();

    if(record.type != RECORD_WRITE)
        throw "Record type doesn't match";

    if(record.pid != pid && pid != -1)
        throw "Process id doesn't match";

    if(record.path != path)
        throw "Path doesn't match";

    if(record.len != len)
        throw "Write length doesn't match";
}

void TestLogger::expect_close(pid_t pid, std::string path) {
    if(!m_records.size())
        throw "Record list is empty";
    
    auto record = m_records.front();
    m_records.pop_front();

    if(record.type != RECORD_CLOSE)
        throw "Record type doesn't match";

    if(record.pid != pid && pid != -1)
        throw "Process id doesn't match";

    if(record.path != path)
        throw "Path doesn't match";
}

void TestLogger::expect_empty_list() {
    if(m_records.size())
        throw "Record list is not empty";
}