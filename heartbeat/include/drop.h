#include <sys/types.h>
#include <unistd.h>

struct kernel_drop_stats {
        pid_t nfqueue_pid;
        
        // stats
        long udp_errors;
        long netlink_drops;
};

int init_kernel_drop(struct kernel_drop_stats *stats);

int check_kernel_drop(struct kernel_drop_stats *last_stats);

int check_nic_drop();