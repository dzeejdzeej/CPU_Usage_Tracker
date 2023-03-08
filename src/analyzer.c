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
    CPU_info* cpu_info = *(CPU_info**)arg;

    while(true)
    {
        pid_t analyzer_id = (pid_t)syscall(SYS_gettid);
        printf("[%d] Analyzer Thread starts\n", analyzer_id);

        printf("[%d] Analyzer Thread waits for CPU_info access\n", analyzer_id);
        cpu_info_lock(cpu_info);

        if (cpu_info_is_empty(cpu_info) == true)
        {
            printf("[%d] Analyzer Thread waits for Reader to fill CPU_info\n", analyzer_id);
            cpu_info_wait_for_reader(cpu_info);
        }

        printf("[%d] Analyzer Thread can analyze /proc/stats data\n", analyzer_id);
        cpu_usage_calculation(cpu_info);

        cpu_info_call_reader(cpu_info);
        cpu_info_unlock(cpu_info);
    }

    return NULL;
}