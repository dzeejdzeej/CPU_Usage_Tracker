# define _GNU_SOURCE // for syscall purpose

#include <watchdog.h>
#include <reader.h>
#include <analyzer.h>
#include <printer.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <time.h>

static void terminate_program(void);

void* watchdog_thread(void* arg)
{
    (void)arg;

    while(true)
    {
        time_t current_time = time(NULL);
        pid_t watchdog_id = (pid_t)syscall(SYS_gettid);

        if ( (current_time - reader_thread_last_activity) > THREAD_TIMER)
        {
            printf("[%d] Thread Reader has stuck for more than 2 secounds\n", watchdog_id);
            terminate_program();
        }

        if ( (current_time - analyzer_thread_last_activity) > THREAD_TIMER)
        {
            printf("[%d] Thread Analyzer has stuck for more than 2 secounds\n", watchdog_id);
            terminate_program();
        }

        if ( (current_time - printer_thread_last_activity) > THREAD_TIMER)
        {
            printf("[%d] Thread Printer has stuck for more than 2 secounds\n", watchdog_id);
            terminate_program();
        }

        // check just once per TIMEOUT
        sleep(THREAD_TIMER);
    }

    return NULL;
}

static void terminate_program(void)
{
    pthread_cancel(reader);
    pthread_cancel(analyzer);
    pthread_cancel(printer);
    exit(EXIT_FAILURE);
}