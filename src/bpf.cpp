#include "bpf.h"
#include <linux/filter.h>
#include <linux/seccomp.h>
#include <syscall.h>
#include <sys/prctl.h>
#include <stddef.h>

struct sock_filter filter[] = {
    BPF_STMT(BPF_LD | BPF_W | BPF_ABS, (offsetof(struct seccomp_data, nr))),

    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_open, 0, 1),
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_TRACE),

    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_openat, 0, 1),
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_TRACE),

    /*BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_read, 0, 1),
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_TRACE),

    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_write, 0, 1),
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_TRACE),*/

    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_close, 0, 1),
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_TRACE),

    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW)
};

bool BPF::setup() {
    struct sock_fprog prog = {
        sizeof(filter) / sizeof(filter[0]),
        filter
    };

    if(prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0))
        return false;

    if(prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &prog))
        return false;

    return true;
}