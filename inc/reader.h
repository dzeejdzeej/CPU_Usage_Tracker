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
    CPU_stats total;
    CPU_stats core[MAX_NUMBER_OF_CORES];
} CPU_info;

void cpu_stats_parser(CPU_info *cpu);

#endif // READER_H