extern void test_cpu_stats(void);
extern void test_cpu_info(void);
extern void test_cpu_stats_parser(void);

int main(void)
{
    test_cpu_stats();
    test_cpu_info();
    test_cpu_stats_parser();
    
    return 0;
}
