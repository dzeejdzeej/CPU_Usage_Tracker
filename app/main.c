#include <cpu.h>
#include <reader.h>
#include <analyzer.h>
#include <printer.h>
#include <watchdog.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

static void signal_handler(int sig);

CPU_info*     cpu;
CPU_usage*    usage;
CPU_combined* combined;
pthread_t     watchdog;

int main(void)
{
    cpu      = CPU_info_new();
    usage    = CPU_usage_new();
    combined = CPU_combined_new(cpu, usage);

    signal(SIGINT,  &signal_handler);
    signal(SIGTERM, &signal_handler);

    pthread_create(&reader,    NULL, reader_thread,   (void*)&cpu);
    pthread_create(&analyzer,  NULL, analyzer_thread, (void*)&combined);
    pthread_create(&printer,   NULL, printer_thread,  (void*)&usage);
    pthread_create(&watchdog,  NULL, watchdog_thread, NULL);

    pthread_join(reader,    NULL);
    pthread_join(analyzer,  NULL);
    pthread_join(printer,   NULL);
    pthread_join(watchdog,  NULL);

    return 0;
}

static void signal_handler(int sig)
{
    if (sig == SIGINT)
        printf("Program closed by SIGINT signal\n");
    else if (sig ==SIGTERM)
        printf("Program closed by SIGTERM signal\n");

    CPU_info_delete(cpu);
    CPU_usage_delete(usage);
    CPU_combined_delete(combined);

    pthread_cancel(reader);
    pthread_cancel(analyzer);
    pthread_cancel(printer);
    pthread_cancel(watchdog);

    exit(EXIT_SUCCESS);
}