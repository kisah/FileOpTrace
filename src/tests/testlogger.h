#pragma once
#include <string>
#include <deque>
#include "../main/logger.h"

namespace Tests {

/**
 * \brief An enum of all possible operation types
 */
enum RecordType {
    RECORD_OPEN,
    RECORD_OPEN_ERROR,
    RECORD_READ,
    RECORD_WRITE,
    RECORD_CLOSE,
    RECORD_RENAME,
    RECORD_REMOVE,
    RECORD_MKDIR
};

/**
 * \brief A struct that represents an operation
 * 
 * Instances of this struct get put into the queue of operations
 */
struct OpRecord {
    /**
     * \brief Constructor
     */
    OpRecord(RecordType _type, pid_t _pid, std::string _path, std::string _path2, int _mode, size_t _len, int _error) : type(_type), pid(_pid), path(_path), path2(_path2), mode(_mode), len(_len), error(_error) {}

    /// Operation type
    RecordType type;

    /// Process id
    pid_t pid;

    /// File path
    std::string path;

    /// Second file path (if necessary)
    std::string path2;

    /// Open mode (if necessary)
    int mode;

    /// Operation length (if necessary)
    size_t len;

    /// Error code
    int error;
};

/**
 * \brief A mock Logger for testing most of the functionality
 * 
 * It logs and saves the sequence of operations that the tracee has performed into a queue
 * and then this sequence can be verified using a set of expect_* functions,
 * those functions throw an exception when they encounter an unexpected event or
 * an empty event queue
 * 
 * The start and the end of a test is marked via a a close(-10) call
 */
class TestLogger : public FileOpTrace::Logger {
public:
    virtual void open(std::string binpath, pid_t pid, std::string path, int mode, int fd);
    virtual void open_failed(std::string binpath, pid_t pid, std::string path, int mode, int error);
    virtual void read(std::string binpath, pid_t pid, std::string path, int fd, size_t len);
    virtual void write(std::string binpath, pid_t pid, std::string path, int fd, size_t len);
    virtual void rename(std::string binpath, pid_t pid, std::string old_path, std::string new_path);
    virtual void close(std::string binpath, pid_t pid, std::string path, int fd);
    virtual void remove(std::string binpath, pid_t pid, std::string path);
    virtual void mkdir(std::string binpath, pid_t pid, std::string path);

    /**
     * \brief Verifies that the first operation in the queue is an open operation with the specified parameters
     * \param pid Process id (or -1 for any process)
     * \param path File path
     * \param mode Open mode (r/w or both)
     * \throw std::string Description of the operation type/parameters mismatch
     */
    void expect_open(pid_t pid, std::string path, int mode);

    /**
     * \brief Verifies that the first operation in the queue is an failed open operation with the specified parameters
     * \param pid Process id (or -1 for any process)
     * \param path File path
     * \param mode Open mode (r/w or both)
     * \param error Error code
     * \throw std::string Description of the operation type/parameters mismatch
     */
    void expect_open_failed(pid_t pid, std::string path, int mode, int error);

    /**
     * \brief Verifies that the first operation in the queue is a read operation with the specified parameters
     * \param pid Process id (or -1 for any process)
     * \param path File path
     * \param len The number of read bytes
     * \throw std::string Description of the operation type/parameters mismatch
     */
    void expect_read(pid_t pid, std::string path, size_t len);

    /**
     * \brief Verifies that the first operation in the queue is a write operation with the specified parameters
     * \param pid Process id (or -1 for any process)
     * \param path File path
     * \param len The number of read bytes
     * \throw std::string Description of the operation type/parameters mismatch
     */
    void expect_write(pid_t pid, std::string path, size_t len);

    /**
     * \brief Verifies that the first operation in the queue is a close operation with the specified parameters
     * \param pid Process id (or -1 for any process)
     * \param path File path
     * \throw std::string Description of the operation type/parameters mismatch
     */
    void expect_close(pid_t pid, std::string path);

    /**
     * \brief Verifies that the first operation in the queue is a rename operation with the specified parameters
     * \param pid Process id (or -1 for any process)
     * \param old_path Old file path
     * \param new_path New file path
     * \throw std::string Description of the operation type/parameters mismatch
     */
    void expect_rename(pid_t pid, std::string old_path, std::string new_path);

    /**
     * \brief Verifies that the first operation in the queue is a remove operation with the specified parameters
     * \param pid Process id (or -1 for any process)
     * \param path File path
     * \throw std::string Description of the operation type/parameters mismatch
     */
    void expect_remove(pid_t pid, std::string path);

    /**
     * \brief Verifies that the first operation in the queue is a mkdir operation with the specified parameters
     * \param pid Process id (or -1 for any process)
     * \param path Directory path
     * \throw std::string Description of the operation type/parameters mismatch
     */
    void expect_mkdir(pid_t pid, std::string path);

    /**
     * \brief Verifies that the operation que is empty
     * \throw std::string Description of the operation type/parameters mismatch
     */
    void expect_empty_list();
private:
    /// A boolean flag, that specifies whether the operations should be put into the queue
    bool m_active = false;

    /// The operation queue
    std::deque<OpRecord> m_records;
};

}