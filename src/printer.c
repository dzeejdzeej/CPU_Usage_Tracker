# define _GNU_SOURCE // for syscall purpose

#include <printer.h>
#include <cpu.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>

pthread_t printer;
volatile time_t printer_thread_last_activity = 0;
volatile sig_atomic_t terminate_printer = 0;

void* printer_thread(void* arg)
{
    CPU_usage* cpu_usage = *(CPU_usage**)arg;

    while(terminate_printer == 0)
    {
        printer_thread_last_activity = time(NULL);

        pid_t printer_id = (pid_t)syscall(SYS_gettid);
        (void) printer_id; // eventually just for logger puropose
        //printf("[%d] Printer Thread starts\n", printer_id);

        cpu_usage_lock(cpu_usage);

        if (cpu_usage_is_empty(cpu_usage) == true)
        {
            //printf("[%d] Printer Thread waits for Analyzer to fill CPU_usage\n", printer_id);
            cpu_usage_wait_for_analyzer(cpu_usage);
        }

        //printf("[%d] Printer Thread can print /proc/stats data\n", printer_id);
        // call main functionality of the thread
        cpu_usage_print(cpu_usage);

        cpu_usage_call_analyzer(cpu_usage);
        cpu_usage_unlock(cpu_usage);

        //printf("[%d] Printer Thread finished\n", printer_id);
    }

    return NULL;
}