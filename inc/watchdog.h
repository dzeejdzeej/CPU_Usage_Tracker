#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <signal.h>

#define THREAD_TIMER 2

extern volatile sig_atomic_t terminate_watchdog;

void* watchdog_thread(void* arg);

#endif // WATCHDOG_H