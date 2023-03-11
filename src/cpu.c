#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cpu.h>

unsigned long long prev_idle_total     = 0;
unsigned long long prev_non_idle_total = 0;
unsigned long long prev_idle_cpu[MAX_NUMBER_OF_CORES]     = {0};
unsigned long long prev_non_idle_cpu[MAX_NUMBER_OF_CORES] = {0};

CPU_stats* CPU_stats_new(void)
{
    CPU_stats* const stats = (CPU_stats*) calloc(1, sizeof(*stats));
    if (stats == NULL)
        return NULL;

    return stats;
}
void CPU_stats_delete(CPU_stats* stats)
{
    if (stats == NULL)
        return;
    
    free(stats);
}

bool cpu_info_is_empty(const CPU_info* info)
{
    return info->is_filled == false;
}

bool cpu_info_is_filled(const CPU_info* info)
{
    return info->is_filled == true;
}

CPU_info* CPU_info_new(void)
{
    CPU_info* info = (CPU_info*) calloc(1, sizeof(*info));
    if (info == NULL)
        return NULL;
    
    info->total = CPU_stats_new();
    
    for (int i = 0; i < MAX_NUMBER_OF_CORES; ++i)
    {
        info->core[i] = CPU_stats_new();
    }

    info->is_filled = false;

    return info;
}

void CPU_info_delete(CPU_info* info)
{
    if (info->total == NULL)
        return;

    free(info->total);

    for (int i = 0; i < MAX_NUMBER_OF_CORES; ++i)
    {
        if (info->core[i] == NULL)
            return;
        
        free(info->core[i]);
    }

    if (info == NULL)
        return;
    
    free(info);
}

CPU_usage* CPU_usage_new(void)
{
    CPU_usage* usage = (CPU_usage*) calloc(1, sizeof(*usage));
    if (usage == NULL)
        return NULL;

    return usage;
}

void CPU_usage_delete(CPU_usage* usage)
{
    if (usage == NULL)
        return;

    free(usage);
}

CPU_combined* CPU_combined_new(CPU_info* info, CPU_usage* usage)
{
    CPU_combined* combined = (CPU_combined*)malloc(sizeof(*combined));

    if (combined == NULL)
        return NULL;

    combined->info = info;
    combined->usage = usage;

    return combined;
}

void CPU_combined_delete(CPU_combined* combined)
{
    if (combined == NULL)
        return;

    free(combined);
}

void cpu_stats_parser(CPU_info *cpu) 
{
    char line_buffer[256];
    CPU_stats* stats = CPU_stats_new();

    FILE *proc_file = fopen("/proc/stat", "r");
    if (proc_file == NULL) 
    {
        printf("Failed to open /proc/stat\n");
        return;
    }

    /* fgets reads a line from the /proc/stat and stores it in line_buffer
       if fgets successfully reads a line, it returns the pointer to the line_buffer which is non-null
       if the end of file is reached fgets returns null pointer
       while loop repeatedly call fgets until we've read all the lines in the file
    */
    while (fgets(line_buffer, sizeof(line_buffer), proc_file)) 
    {
        // "cpu " indicates line where we got accumulated values from each CPU
        if (strncmp(line_buffer, "cpu ", 4) == 0) 
        {
            sscanf(line_buffer + 4, "%llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
                   &stats->user,
                   &stats->nice,
                   &stats->system,
                   &stats->idle,
                   &stats->iowait,
                   &stats->irq,
                   &stats->softirq,
                   &stats->steal,
                   &stats->guest,
                   &stats->guest_nice);
            
            memcpy(cpu->total, stats, sizeof(*stats));
        }
        // "cpuN" indicates lines where we got values for specific CPUs
        else if (strncmp(line_buffer, "cpu", 3) == 0) 
        {
            int core_id;
            sscanf(line_buffer + 3, "%d %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
                   &core_id,
                   &stats->user,
                   &stats->nice,
                   &stats->system,
                   &stats->idle,
                   &stats->iowait,
                   &stats->irq,
                   &stats->softirq,
                   &stats->steal,
                   &stats->guest,
                   &stats->guest_nice);

            if (core_id >= 0 && core_id < MAX_NUMBER_OF_CORES) 
            {
                memcpy(cpu->core[core_id], stats, sizeof(*stats));
            }
        }
        else
        {
            // just first lines contains cpu information, if line without "cpu" coccurs, not needed to analyse rest of file
            break;
        }
    }

    cpu->is_filled = true;

    CPU_stats_delete(stats);
    fclose(proc_file);
}

void cpu_usage_calculation(CPU_info* cpu, CPU_usage* usage)
{
    // Calculates total (summary of all CPUs)
    unsigned long long idle_total     = cpu->total->idle + cpu->total->iowait;
    unsigned long long non_idle_total = cpu->total->user + cpu->total->nice + cpu->total->system +
                                        cpu->total->softirq + cpu->total->steal;

    unsigned long long all_total      = idle_total      + non_idle_total;
    unsigned long long prev_all_total = prev_idle_total + prev_non_idle_total;

    double all_by_sec_total  = (double)all_total  - (double)prev_all_total;
    double idle_by_sec_total = (double)idle_total - (double)prev_idle_total;
    
    usage->cpu_percentage_total = (all_by_sec_total - idle_by_sec_total) / all_by_sec_total * 100.0;

    printf("CPU percentage for total (sum of all CPUs): %.2f%%\n", usage->cpu_percentage_total);

    // save this values for another calculation
    prev_idle_total     = idle_total;
    prev_non_idle_total = non_idle_total;


    // Calculates per CPU
    unsigned long long idle_cpu[MAX_NUMBER_OF_CORES]           = {0};
    unsigned long long non_idle_cpu[MAX_NUMBER_OF_CORES]       = {0};
    unsigned long long all_cpu[MAX_NUMBER_OF_CORES]            = {0};
    unsigned long long prev_all_cpu[MAX_NUMBER_OF_CORES]       = {0};
    double             all_by_sec_cpu[MAX_NUMBER_OF_CORES]     = {0};
    double             idle_by_sec_cpu[MAX_NUMBER_OF_CORES]    = {0};

    for (int i = 0; i < MAX_NUMBER_OF_CORES; ++i)
    {
        idle_cpu[i]     = cpu->core[i]->idle + cpu->core[i]->iowait;
        non_idle_cpu[i] = cpu->core[i]->user + cpu->core[i]->nice + cpu->core[i]->system +
                          cpu->core[i]->softirq + cpu->core[i]->steal;

        all_cpu[i]      = idle_cpu[i]      + non_idle_cpu[i];
        prev_all_cpu[i] = prev_idle_cpu[i] + prev_non_idle_cpu[i];

        all_by_sec_cpu[i]  = (double)all_cpu[i]  - (double)prev_all_cpu[i];
        idle_by_sec_cpu[i] = (double)idle_cpu[i] - (double)prev_idle_cpu[i];

        usage->cpu_percentage_cpu[i] = 
            (all_by_sec_cpu[i] - idle_by_sec_cpu[i]) / all_by_sec_cpu[i] * 100.0;

        printf("CPU[%d] percentage: %.2f%%\n", i, usage->cpu_percentage_cpu[i]);

        // save this values for another calculation
        prev_idle_cpu[i]     = idle_cpu[i];
        prev_non_idle_cpu[i] = non_idle_cpu[i];
    }

    // indicates that new data has been analyzed and now we're waitng for new portion
    cpu->is_filled = false;
}

// Multithread functions for reader <-> analyzer
void cpu_info_lock(CPU_info* info)
{
    pthread_mutex_lock(&info->mutex);
}

void cpu_info_unlock(CPU_info* info)
{
    pthread_mutex_unlock(&info->mutex);
}

void cpu_info_call_reader(CPU_info* info)
{
    pthread_cond_signal(&info->can_read);
}

void cpu_info_call_analyzer(CPU_info* info)
{
    pthread_cond_signal(&info->can_analyze);
}

void cpu_info_wait_for_reader(CPU_info* info)
{
    pthread_cond_wait(&info->can_analyze, &info->mutex);
}

void cpu_info_wait_for_analyzer(CPU_info* info)
{
    pthread_cond_wait(&info->can_read, &info->mutex);
}

// Multithread functions for analyzer <-> printer
void cpu_usage_lock(CPU_usage* usage)
{
    pthread_mutex_lock(&usage->mutex);
}
void cpu_usage_unlock(CPU_usage* usage)
{
    pthread_mutex_unlock(&usage->mutex);
}

void cpu_usage_call_analyzer(CPU_usage* usage)
{
    pthread_cond_signal(&usage->can_analyze);
}

void cpu_usage_call_printer(CPU_usage* usage)
{
    pthread_cond_signal(&usage->can_print);
}

void cpu_usage_wait_for_analyzer(CPU_usage* usage)
{
    pthread_cond_wait(&usage->can_print, &usage->mutex);
}

void cpu_usage_wait_for_printer(CPU_usage* usage)
{
    pthread_cond_wait(&usage->can_analyze, &usage->mutex);
}