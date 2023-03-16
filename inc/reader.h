#ifndef READER_H
#define READER_H

#include <pthread.h>
#include <time.h>

extern pthread_t reader;
extern time_t reader_thread_last_activity;

void* reader_thread(void* arg);

#endif // READER_H