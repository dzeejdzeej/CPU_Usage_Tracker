#ifndef PRINTER_H
#define PRINTER_H

#include <pthread.h>
#include <time.h>
#include <signal.h>

extern pthread_t printer;
extern volatile time_t printer_thread_last_activity;
extern volatile sig_atomic_t terminate_printer;

void* printer_thread(void* arg);

#endif // PRINTER_H