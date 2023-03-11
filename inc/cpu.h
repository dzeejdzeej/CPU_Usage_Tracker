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

// struct between reader and analyzer
typedef struct CPU_info 
{
    bool is_filled;
    CPU_stats* total;
    CPU_stats* core[MAX_NUMBER_OF_CORES];
    pthread_mutex_t mutex;
    pthread_cond_t can_read;
    pthread_cond_t can_analyze;
} CPU_info;

// struct between analyzer and printer
typedef struct CPU_usage
{
    bool is_filled;
    double cpu_percentage_total;
    double cpu_percentage_cpu[MAX_NUMBER_OF_CORES];
    pthread_mutex_t mutex;
    pthread_cond_t can_analyze;
    pthread_cond_t can_print;
} CPU_usage;

typedef struct CPU_combined
{
    CPU_info* info;     // input for analyzer_thread
    CPU_usage* usage;   // output of analyzer_thread
} CPU_combined;

CPU_stats* CPU_stats_new(void);
void CPU_stats_delete(CPU_stats* stats);

CPU_info* CPU_info_new(void);
void CPU_info_delete(CPU_info* info);

CPU_usage* CPU_usage_new(void);
void CPU_usage_delete(CPU_usage* usage);

CPU_combined* CPU_combined_new(CPU_info* info, CPU_usage* usage);
void CPU_combined_delete(CPU_combined* combined);

bool cpu_info_is_empty(const CPU_info* info);
bool cpu_info_is_filled(const CPU_info* info);
bool cpu_usage_is_empty(const CPU_usage* usage);
bool cpu_usage_is_filled(const CPU_usage* usage);

void cpu_stats_parser(CPU_info *cpu);
void cpu_usage_calculation(CPU_info* cpu, CPU_usage* usage);
void cpu_usage_print(CPU_usage* cpu_usage);

// Multithread functions for reader <-> analyzer
void cpu_info_lock(CPU_info* info);
void cpu_info_unlock(CPU_info* info);
void cpu_info_call_reader(CPU_info* info);
void cpu_info_call_analyzer(CPU_info* info);
void cpu_info_wait_for_reader(CPU_info* info);
void cpu_info_wait_for_analyzer(CPU_info* info);

// Multithread functions for analyzer <-> printer
void cpu_usage_lock(CPU_usage* usage);
void cpu_usage_unlock(CPU_usage* usage);
void cpu_usage_call_analyzer(CPU_usage* usage);
void cpu_usage_call_printer(CPU_usage* usage);
void cpu_usage_wait_for_analyzer(CPU_usage* usage);
void cpu_usage_wait_for_printer(CPU_usage* usage);

#endif // CPU_H
