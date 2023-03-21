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

volatile sig_atomic_t terminate_watchdog = 0;

static void signal_termination_handler(int sig);

void* watchdog_thread(void* arg)
{
    (void)arg;

    while(terminate_watchdog == 0)
    {
        time_t current_time = time(NULL);
        pid_t watchdog_id = (pid_t)syscall(SYS_gettid);
        (void) watchdog_id; // eventually just for logger puropose

        if ( (current_time - reader_thread_last_activity) > THREAD_TIMER)
        {
            printf("[%d] Thread Reader has stuck for more than 2 secounds\n", watchdog_id);
            signal_termination_handler(SIGTERM);
        }

        if ( (current_time - analyzer_thread_last_activity) > THREAD_TIMER)
        {
            printf("[%d] Thread Analyzer has stuck for more than 2 secounds\n", watchdog_id);
            signal_termination_handler(SIGTERM);
        }

        if ( (current_time - printer_thread_last_activity) > THREAD_TIMER)
        {
            printf("[%d] Thread Printer has stuck for more than 2 secounds\n", watchdog_id);
            signal_termination_handler(SIGTERM);
        }

        // check just once per TIMEOUT
        sleep(THREAD_TIMER);
    }

    return NULL;
}


static void signal_termination_handler(int sig)
{
    terminate_reader   = 1;
    terminate_analyzer = 1;
    terminate_printer  = 1;
    terminate_watchdog = 1;

    if (sig == SIGINT)
        printf("Program closed by SIGINT signal\n");
    else if (sig ==SIGTERM)
        printf("Program closed by SIGTERM signal\n");

    exit(EXIT_SUCCESS);
}