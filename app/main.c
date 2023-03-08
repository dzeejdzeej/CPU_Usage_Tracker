#include <cpu.h>
#include <reader.h>
#include <analyzer.h>
#include <stdio.h>
#include <pthread.h>

int main(void)
{
    CPU_info* cpu = CPU_info_new();

    pthread_t read;
    pthread_t analyze;

    pthread_create(&read, NULL, reader_thread, (void*)&cpu);
    pthread_create(&analyze, NULL, analyzer_thread, (void*)&cpu);

    pthread_join(read, NULL);
    pthread_join(analyze, NULL);

    CPU_info_delete(cpu);

    return 0;
}
