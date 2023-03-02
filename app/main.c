#include <cpu.h>
#include <stdio.h>

int main(void)
{
    CPU_info* cpu = CPU_info_new();
    cpu_stats_parser(cpu);

    printf("Total CPU usage: user=%llu, nice=%llu, system=%llu, idle=%llu, iowait=%llu, irq=%llu, "
            "softirq=%llu, steal=%llu, guest=%llu, guest_nice=%llu\n",
            cpu->total->user,
            cpu->total->nice,
            cpu->total->system,
            cpu->total->idle,
            cpu->total->iowait,
            cpu->total->irq,
            cpu->total->softirq,
            cpu->total->steal,
            cpu->total->guest,
            cpu->total->guest_nice);

    for (int i = 0; i < MAX_NUMBER_OF_CORES; i++) 
    {
        printf("CPU %d usage: user=%llu, nice=%llu, system=%llu, idle=%llu, iowait=%llu, irq=%llu, "
                "softirq=%llu, steal=%llu, guest=%llu, guest_nice=%llu\n",
                i,
                cpu->core[i]->user,
                cpu->core[i]->nice,
                cpu->core[i]->system,
                cpu->core[i]->idle,
                cpu->core[i]->iowait,
                cpu->core[i]->irq,
                cpu->core[i]->softirq,
                cpu->core[i]->steal,
                cpu->core[i]->guest,
                cpu->core[i]->guest_nice);               
    }

    CPU_info_delete(cpu);

    return 0;
}
