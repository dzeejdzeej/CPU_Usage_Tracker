#ifndef PRINTER_H
#define PRINTER_H

#include <pthread.h>
#include <time.h>

extern pthread_t printer;
extern time_t printer_thread_last_activity;

void* printer_thread(void* arg);

#endif // PRINTER_H