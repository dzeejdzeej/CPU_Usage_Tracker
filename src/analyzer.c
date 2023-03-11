# define _GNU_SOURCE // for syscall purpose

#include <analyzer.h>
#include <cpu.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>

void* analyzer_thread(void* arg)
{
    CPU_combined* cpu_combined = *(CPU_combined**)arg;

    while(true)
    {
        pid_t analyzer_id = (pid_t)syscall(SYS_gettid);
        printf("[%d] Analyzer Thread starts\n", analyzer_id);

        printf("[%d] Analyzer Thread waits for CPU_info access\n", analyzer_id);
        cpu_info_lock(cpu_combined->info);

        if (cpu_info_is_empty(cpu_combined->info) == true)
        {
            printf("[%d] Analyzer Thread waits for Reader to fill CPU_info\n", analyzer_id);
            cpu_info_wait_for_reader(cpu_combined->info);
        }

        printf("[%d] Analyzer Thread can analyze /proc/stats data\n", analyzer_id);
        // call main functionality of the thread
        cpu_usage_calculation(cpu_combined->info, cpu_combined->usage);

        cpu_info_call_reader(cpu_combined->info);
        cpu_info_unlock(cpu_combined->info);
    }

    return NULL;
}