#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cpu.h>

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

void cpu_usage_calculation(CPU_info *cpu)
{
    // @todo needs previous values for proper calculation
    unsigned long long idle = cpu->total->idle + cpu->total->iowait;
    unsigned long long non_idle = cpu->total->user + cpu->total->nice + cpu->total->system +
                                  cpu->total->softirq + cpu->total->steal;

    unsigned long long total = idle + non_idle;
    double cpu_percentage = ( ((double)total - (double)idle) / (double)total ) * 100.0;
    printf("CPU percentage: %f\n", cpu_percentage);

    cpu->is_filled = false;
}

// Multithread functions
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