#include <stdio.h>
#include <assert.h>
#include <cpu.h>

void test_cpu_stats(void);
void test_cpu_info(void);
void test_cpu_usage(void);
void test_cpu_combined(void);
void test_cpu_stats_parser(void);
void test_cpu_usage_calculation(void);

void test_cpu_stats(void)
{
    CPU_stats* stats = CPU_stats_new();

    assert(stats != NULL);
    assert(stats->iowait == 0);
    assert(stats->user == 0);

    CPU_stats_delete(stats);
}

void test_cpu_info(void)
{
    CPU_info* info = CPU_info_new();

    assert(info != NULL);
    assert(info->total != NULL);
    assert(info->total->user == 0);
    assert(info->total->system == 0);
    assert(info->is_filled == false);

    for (int i = 0; i < MAX_NUMBER_OF_CORES; ++i)
    {
        assert(info->core[i] != NULL);
        assert(info->core[i]->user == 0);
        assert(info->core[i]->softirq == 0);
    }

    CPU_info_delete(info);
}

void test_cpu_usage(void)
{
    CPU_usage* usage = CPU_usage_new();

    assert(usage != NULL);
    assert(usage->cpu_percentage_total == 0);

    for (int i = 0; i < MAX_NUMBER_OF_CORES; ++i)
    {
        assert(usage->cpu_percentage_cpu[i] == 0);
    }

    CPU_usage_delete(usage);
}

void test_cpu_combined(void)
{
    CPU_info* info = CPU_info_new();
    CPU_usage* usage = CPU_usage_new();

    CPU_combined* combined = CPU_combined_new(info, usage);

    assert(combined != NULL);
    assert(combined->info != NULL);
    assert(combined->usage != NULL);

    CPU_info_delete(info);
    CPU_usage_delete(usage);
    CPU_combined_delete(combined);
}

void test_cpu_stats_parser(void)
{
    CPU_info* info = CPU_info_new();

    assert(info->total->user == 0);
    assert(info->total->system == 0);
    assert(info->is_filled == false);
    
    for (int i = 0; i < MAX_NUMBER_OF_CORES; ++i)
    {
        assert(info->core[i]->user == 0);
        assert(info->core[i]->system == 0);
    }

    cpu_stats_parser(info);

    assert(info->total->user > 0);
    assert(info->total->system > 0);
    assert(info->is_filled == true);

    CPU_info_delete(info);
}

void test_cpu_usage_calculation(void)
{
    CPU_info*  info  = CPU_info_new();
    CPU_usage* usage = CPU_usage_new();

    *(info->total) = (CPU_stats) 
    { 
        .user       = 500,
        .nice       = 250,
        .system     = 500,
        .idle       = 500,
        .iowait     = 1000,
        .irq        = 0,
        .softirq    = 250,
        .steal      = 0,
        .guest      = 0,
        .guest_nice = 0
    };

    for (int i = 0; i < MAX_NUMBER_OF_CORES; ++i) 
    {
        *(info->core[i]) = (CPU_stats) 
        { 
            .user       = 1000,
            .nice       = 250,
            .system     = 250,
            .idle       = 750,
            .iowait     = 250,
            .irq        = 0,
            .softirq    = 500,
            .steal      = 1000,
            .guest      = 0,
            .guest_nice = 0
        };
    }

    /*
        TOTAL:
        idle_total = idle + iowait
        idle_total = 500  + 1000   = 1500
        non_idle_total = user + nice + system + softirq + steal
        non_idle_total = 500  + 250  + 500    + 250     + 0     = 1500
        all_total = idle_total + non_idle_total
        all_total = 1500       + 1500           = 3000
        cpu_percentage_total = (all_total - idle_total) / all_total * 100.0
        cpu_percentage_total = (3000      - 1500      ) / 3000      * 100.0 = 50 %

        CPU[0-7]:
        idle_total = 750 + 250 = 1000
        non_idle_total = 1000 + 250 + 250 + 500 + 1000 = 3000
        all_total = 1000 + 3000 = 4000
        cpu_percentage_total = (4000 - 1000) / 4000 * 100.0 = 75 %
    */

    cpu_usage_calculation(info, usage);

    assert(usage->cpu_percentage_total == 50);

    for (int i = 0; i < MAX_NUMBER_OF_CORES; ++i)
    {
        assert(usage->cpu_percentage_cpu[i] == 75);
    }

    CPU_info_delete(info);
    CPU_usage_delete(usage);
}
