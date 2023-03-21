#ifndef READER_H
#define READER_H

#include <pthread.h>
#include <time.h>
#include <signal.h>

extern pthread_t reader;
extern volatile time_t reader_thread_last_activity;
extern volatile sig_atomic_t terminate_reader;

void* reader_thread(void* arg);

#endif // READER_H