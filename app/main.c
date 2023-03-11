#include <cpu.h>
#include <reader.h>
#include <analyzer.h>
#include <stdio.h>
#include <pthread.h>

int main(void)
{
    CPU_info* cpu = CPU_info_new();
    CPU_usage* usage = CPU_usage_new();
    CPU_combined* combined = CPU_combined_new(cpu, usage);

    pthread_t read;
    pthread_t analyze;

    pthread_create(&read, NULL, reader_thread, (void*)&cpu);
    pthread_create(&analyze, NULL, analyzer_thread, (void*)&combined);

    pthread_join(read, NULL);
    pthread_join(analyze, NULL);

    CPU_info_delete(cpu);
    CPU_usage_delete(usage);
    CPU_combined_delete(combined);

    return 0;
}
