#pragma once
#include <string>

namespace FileOpTrace {

/**
 * \brief A skeleton of a logger
 * \warning Contains pure virtual functions only
 * 
 * A logger class is responsible for outputting messages
 * about file operations in a nice format
 */
class Logger {
public:
    /**
     * \brief Constructor
     */
    Logger() {}

    /**
     * \brief Destructor
     */
    virtual ~Logger() {}

    /**
     * \brief Gets called when a file/directory is opened
     * \param binpath Executable path for a given process
     * \param pid Process id
     * \param path File path
     * \param mode Open mode (r/w or both)
     * \param fd A file descriptor for this file
     */
    virtual void open(std::string binpath, pid_t pid, std::string path, int mode, int fd) = 0;

    /**
     * \brief Gets called when a file/directory can't be opened
     * \param binpath Executable path for a given process
     * \param pid Process id
     * \param path File path
     * \param mode Open mode (r/w or both)
     * \param error Error code
     */
    virtual void open_failed(std::string binpath, pid_t pid, std::string path, int mode, int error) = 0;

    /**
     * \brief Gets called when a file is read
     * \param binpath Executable path for a given process
     * \param pid Process id
     * \param path File path
     * \param fd A file descriptor for this file
     * \param len The number of read bytes
     */
    virtual void read(std::string binpath, pid_t pid, std::string path, int fd, size_t len) = 0;

    /**
     * \brief Gets called when a file is written
     * \param binpath Executable path for a given process
     * \param pid Process id
     * \param path File path
     * \param fd A file descriptor for this file
     * \param len The number of written bytes
     */
    virtual void write(std::string binpath, pid_t pid, std::string path, int fd, size_t len) = 0;

    /**
     * \brief Gets called when a file descriptor is closed
     * \param binpath Executable path for a given process
     * \param pid Process id
     * \param path File path
     * \param fd A file descriptor for this file
     */
    virtual void close(std::string binpath, pid_t pid, std::string path, int fd) = 0;

    /**
     * \brief Gets called when a file is renamed
     * \param binpath Executable path for a given process
     * \param pid Process id
     * \param old_path Old file path
     * \param new_path New file path
     */
    virtual void rename(std::string binpath, pid_t pid, std::string old_path, std::string new_path) = 0;

    /**
     * \brief Gets called when a file/directory is removed
     * \param binpath Executable path for a given process
     * \param pid Process id
     * \param path File path
     */
    virtual void remove(std::string binpath, pid_t pid, std::string path) = 0;

    /**
     * \brief Gets called when a directory is created
     * \param binpath Executable path for a given process
     * \param pid Process id
     * \param path File path
     */
    virtual void mkdir(std::string binpath, pid_t pid, std::string path) = 0;
};

}