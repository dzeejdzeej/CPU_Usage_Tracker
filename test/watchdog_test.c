#include <cpu.h>
#include <reader.h>
#include <analyzer.h>
#include <printer.h>
#include <watchdog.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void test_watchdog(void);

void test_watchdog(void)
{
    pthread_t watchdog;
    pthread_cond_t never_accepted_cond;

    CPU_info*     cpu      = CPU_info_new();
    CPU_usage*    usage    = CPU_usage_new();
    CPU_combined* combined = CPU_combined_new(cpu, usage);

    pthread_create(&reader,    NULL, reader_thread,   (void*)&cpu);
    pthread_create(&analyzer,  NULL, analyzer_thread, (void*)&combined);
    pthread_create(&printer,   NULL, printer_thread,  (void*)&usage);
    pthread_create(&watchdog,  NULL, watchdog_thread, NULL);

    // sleep a little bit to program start working
    sleep(4);

    // force thread to infinity wait
    pthread_cond_wait(&never_accepted_cond, &cpu->mutex);

    pthread_join(reader,    NULL);
    pthread_join(analyzer,  NULL);
    pthread_join(printer,   NULL);
    pthread_join(watchdog,  NULL);


    CPU_info_delete(cpu);
    CPU_usage_delete(usage);
    CPU_combined_delete(combined);
}
