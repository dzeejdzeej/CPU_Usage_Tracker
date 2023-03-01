#include <stdio.h>
#include <assert.h>
#include <reader.h>

void test_reader_cpu_stats(void);
void test_reader_cpu_info(void);

void test_reader_cpu_stats(void)
{
    CPU_stats* stats = CPU_stats_new();

    assert(stats != NULL);
    assert(stats->iowait == 0);
    assert(stats->user == 0);

    CPU_stats_delete(stats);
}

void test_reader_cpu_info(void)
{
    CPU_info* info = CPU_info_new();

    assert(info != NULL);
    assert(info->total != NULL);
    assert(info->total->user == 0);
    assert(info->total->system == 0);
    
    for (int i = 0; i < MAX_NUMBER_OF_CORES; ++i)
    {
        assert(info->core[i] != NULL);
        assert(info->core[i]->user == 0);
        assert(info->core[i]->softirq == 0);
    }

    CPU_info_delete(info);
}
