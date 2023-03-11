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
    // tbd
}
