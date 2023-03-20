# define _GNU_SOURCE // for syscall purpose

#include <analyzer.h>
#include <cpu.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>

pthread_t analyzer;
time_t analyzer_thread_last_activity = 0;

void* analyzer_thread(void* arg)
{
    CPU_combined* cpu_combined = *(CPU_combined**)arg;

    while(true)
    {
        analyzer_thread_last_activity = time(NULL);

        pid_t analyzer_id = (pid_t)syscall(SYS_gettid);
        (void) analyzer_id; // eventually just for logger puropose
        //printf("[%d] Analyzer Thread starts\n", analyzer_id);

        cpu_info_lock(cpu_combined->info);

        if (cpu_info_is_empty(cpu_combined->info) == true)
        {
            //printf("[%d] Analyzer Thread waits for Reader to fill CPU_info\n", analyzer_id);
            cpu_info_wait_for_reader(cpu_combined->info);
        }

        cpu_usage_lock(cpu_combined->usage);

        if (cpu_usage_is_filled(cpu_combined->usage) == true)
        {
            //printf("[%d] Analyzer Thread waits for Printer to print the data\n", analyzer_id);
            cpu_usage_wait_for_printer(cpu_combined->usage);
        }

        //printf("[%d] Analyzer Thread can analyze /proc/stats data and write to CPU_usage\n", analyzer_id);
        // call main functionality of the thread
        cpu_usage_calculation(cpu_combined->info, cpu_combined->usage);

        cpu_info_call_reader(cpu_combined->info);
        cpu_usage_call_printer(cpu_combined->usage);

        cpu_info_unlock(cpu_combined->info);
        cpu_usage_unlock(cpu_combined->usage);

        //printf("[%d] Analyzer Thread finished\n", analyzer_id);
    }

    return NULL;
}