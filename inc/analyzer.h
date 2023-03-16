#ifndef ANALYZER_H
#define ANALYZER_H

#include <pthread.h>
#include <time.h>

extern pthread_t analyzer;
extern time_t analyzer_thread_last_activity;

void* analyzer_thread(void* arg);

#endif // ANALYZER_H