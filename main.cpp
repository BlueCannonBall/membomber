#include "config.hpp"
#include "manager.hpp"
#include <chrono>
#include <stdexcept>
#include <string>
#include <sys/mman.h>
#include <thread>

int main() {
    // Prevent Memrescue's memory from being paged to the swap area
    mlockall(MCL_CURRENT);
    mlockall(MCL_FUTURE);

    ResourceManager manager;
    manager.adjust_oom_score(getpid(), -1000);
    manager.adjust_niceness(-20);
    manager.log("Memrescue is running");

    for (;;) {
        CPUStats start_cpu_stats = manager.cpu_stats();
        std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
        CPUStats end_cpu_stats = manager.cpu_stats();
        MemoryInfo memory_info = manager.info();
        auto now = std::chrono::steady_clock::now();

        double iowait_percentage = (double) (end_cpu_stats.iowait_time - start_cpu_stats.iowait_time) / (end_cpu_stats.total_time - start_cpu_stats.total_time);
        double memory_percentage = 1. - (double) memory_info.available / memory_info.total;

#ifdef DEBUG
        manager.log("{ iowait_percentage: " + std::to_string(iowait_percentage) + ", memory_percentage: " + std::to_string(memory_percentage) + " }");
#endif

        if (iowait_percentage > IOWAIT_THRES) {
            if (memory_percentage > MAX_MEMORY_USAGE) {
                manager.kill_process(manager.get_highest().pid);

                if (now - manager.clears.cache > std::chrono::milliseconds(CACHE_TIMEOUT) && memory_info.cached) {
                    manager.drop_caches();
                }
            } else if (now - manager.clears.swap > std::chrono::milliseconds(SWAP_TIMEOUT) &&
                       memory_info.total_swap &&
                       memory_info.free_swap < memory_info.total_swap &&
                       memory_info.available > memory_info.total_swap - memory_info.free_swap) {
                manager.clear_swap();
            }
        }
    }

    return 0;
}
