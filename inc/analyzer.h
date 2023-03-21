#ifndef ANALYZER_H
#define ANALYZER_H

#include <pthread.h>
#include <time.h>
#include <signal.h>

extern pthread_t analyzer;
extern volatile time_t analyzer_thread_last_activity;
extern volatile sig_atomic_t terminate_analyzer;

void* analyzer_thread(void* arg);

#endif // ANALYZER_H