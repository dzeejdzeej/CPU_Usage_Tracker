#ifndef READER_H
#define READER_H

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
    CPU_stats* total;
    CPU_stats* core[MAX_NUMBER_OF_CORES];
} CPU_info;

CPU_stats* CPU_stats_new(void);
void CPU_stats_delete(CPU_stats* stats);

CPU_info* CPU_info_new(void);
void CPU_info_delete(CPU_info* info);

void cpu_stats_parser(CPU_info *cpu);

#endif // READER_H
