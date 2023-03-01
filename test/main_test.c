extern void test_reader_cpu_stats(void);
extern void test_reader_cpu_info(void);

int main(void)
{
    test_reader_cpu_stats();
    test_reader_cpu_info();
    return 0;
}
