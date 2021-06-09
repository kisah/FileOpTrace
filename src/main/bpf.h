#pragma once

namespace PTrace {

/**
 * \brief A small helper class that installs a seccomp filter in all child processes
 * 
 * This class is a part of the c++ wrapper for ptrace
 */
class BPF {
    public:
        /**
         * \brief Install a seccomp filter
         * \return Success status
         */
        static bool setup();
};

}