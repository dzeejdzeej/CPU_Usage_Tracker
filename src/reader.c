# define _GNU_SOURCE // for syscall purpose

#include <reader.h>
#include <cpu.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>

void* reader_thread(void* arg)
{
    CPU_info* cpu_info = *(CPU_info**)arg;

    while(true)
    {
        pid_t reader_id = (pid_t)syscall(SYS_gettid);
        printf("[%d] Reader Thread starts\n", reader_id);

        // Reads data each secound
        sleep(1);

        cpu_info_lock(cpu_info);

        if (cpu_info_is_filled(cpu_info) == true)
        {
            printf("[%d] Reader Thread waits for Analyzer to process CPU_info\n", reader_id);
            cpu_info_wait_for_analyzer(cpu_info);
        }

        printf("[%d] Reader Thread can read and parse /proc/stats data\n", reader_id);
        // call main functionality of the thread
        cpu_stats_parser(cpu_info);

        cpu_info_call_analyzer(cpu_info);
        cpu_info_unlock(cpu_info);

        printf("[%d] Reader Thread finished\n", reader_id);
    }

    return NULL;
}