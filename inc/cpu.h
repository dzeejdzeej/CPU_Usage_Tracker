#ifndef CPU_H
#define CPU_H

#include <pthread.h>
#include <stdbool.h>

#define MAX_NUMBER_OF_CORES 8

typedef struct CPU_stats
{
    unsigned long long user;
    unsigned long long nice;
    unsigned long long system;
    unsigned long long idle;
    unsigned long long iowait;
    unsigned long long irq;
    unsigned long long softirq;
    unsigned long long steal;
    unsigned long long guest;
    unsigned long long guest_nice;
} CPU_stats;

typedef struct CPU_info 
{
    bool is_filled;
    CPU_stats* total;
    CPU_stats* core[MAX_NUMBER_OF_CORES];
    pthread_mutex_t mutex;
    pthread_cond_t can_read;
    pthread_cond_t can_analyze;
} CPU_info;

CPU_stats* CPU_stats_new(void);
void CPU_stats_delete(CPU_stats* stats);

CPU_info* CPU_info_new(void);
void CPU_info_delete(CPU_info* info);

bool cpu_info_is_empty(const CPU_info* info);
bool cpu_info_is_filled(const CPU_info* info);

void cpu_stats_parser(CPU_info *cpu);
void cpu_usage_calculation(CPU_info *cpu);

// Multithread functions
void cpu_info_lock(CPU_info* info);
void cpu_info_unlock(CPU_info* info);
void cpu_info_call_reader(CPU_info* info);
void cpu_info_call_analyzer(CPU_info* info);
void cpu_info_wait_for_reader(CPU_info* info);
void cpu_info_wait_for_analyzer(CPU_info* info);

#endif // CPU_H
