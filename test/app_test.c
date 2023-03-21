#include <cpu.h>
#include <reader.h>
#include <analyzer.h>
#include <printer.h>
#include <watchdog.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

void test_app_stress_one_cpu(void);
static void signal_termination_handler(int sig);

CPU_info*     cpu_test;
CPU_usage*    usage_test;
CPU_combined* combined_test;

void test_app_stress_one_cpu(void)
{
    pthread_t watchdog;

    cpu_test      = CPU_info_new();
    usage_test    = CPU_usage_new();
    combined_test = CPU_combined_new(cpu_test, usage_test);

    pthread_create(&reader,    NULL, reader_thread,   (void*)&cpu_test);
    pthread_create(&analyzer,  NULL, analyzer_thread, (void*)&combined_test);
    pthread_create(&printer,   NULL, printer_thread,  (void*)&usage_test);
    pthread_create(&watchdog,  NULL, watchdog_thread, NULL);

    // use this sleep to stabilize usage cpu values before check
    sleep(2);

    // check if at the start cpu usage is rather low
    assert(usage_test->cpu_percentage_total > 0);
    assert(usage_test->cpu_percentage_total < 5);

    // set timer for while loop 
    const time_t end_time = time(NULL) + 6;

    // set timer for asserts to stabilize usage cpu values before check
    const time_t trigger_asseerts = time(NULL) + 2;


    while (time(NULL) < end_time)
    {
        // Do some work to continuously use one of the CPUs 
        for (int i = 0; i < 1000000; i++) {}

        /*
          Checks if total cpu usage is near 12.5%.
          One of 8 CPUs consume 100%, others has negligible impact, so 1/8 is 12.5%
        */
        if (trigger_asseerts < time(NULL))
        {
            assert(usage_test->cpu_percentage_total > 10);
            assert(usage_test->cpu_percentage_total < 15);
        }
    }

    // check if usage has back to low values after releasing the loop
    sleep(2);
    assert(usage_test->cpu_percentage_total > 0);
    assert(usage_test->cpu_percentage_total < 5);

    // close all threads after test
    signal_termination_handler(SIGTERM);

    pthread_join(reader,    NULL);
    pthread_join(analyzer,  NULL);
    pthread_join(printer,   NULL);
    pthread_join(watchdog,  NULL);
}

static void signal_termination_handler(int sig)
{
    terminate_reader   = 1;
    terminate_analyzer = 1;
    terminate_printer  = 1;
    terminate_watchdog = 1;

    CPU_info_delete(cpu_test);
    CPU_usage_delete(usage_test);
    CPU_combined_delete(combined_test);

    if (sig == SIGINT)
        printf("Program closed by SIGINT signal\n");
    else if (sig ==SIGTERM)
        printf("Program closed by SIGTERM signal\n");

    exit(EXIT_SUCCESS);
}