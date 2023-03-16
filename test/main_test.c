extern void test_cpu_stats(void);
extern void test_cpu_info(void);
extern void test_cpu_usage(void);
extern void test_cpu_combined(void);
extern void test_cpu_stats_parser(void);
extern void test_cpu_usage_calculation(void);
extern void test_watchdog(void);

int main(void)
{
    test_cpu_stats();
    test_cpu_info();
    test_cpu_usage();
    test_cpu_combined();
    test_cpu_stats_parser();
    test_cpu_usage_calculation();

    //test_watchdog();
    
    return 0;
}
