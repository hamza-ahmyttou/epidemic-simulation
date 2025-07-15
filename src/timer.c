#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define TIMER_SIGNAL SIGRTMIN

timer_t timer_id;

static void handle_fatal_error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void timer_handler(int sig, siginfo_t *si, void *uc) {
    (void)sig;
    (void)si;
    (void)uc;
    printf("[TIMER] Timer expired. Performing periodic task...\n");

    // Here you would place your periodic logic (e.g., update simulation step)
}

void init_timer(int interval_sec) {
    struct sigaction sa;
    struct sigevent sev;
    struct itimerspec its;

    // Setup signal handler
    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timer_handler;
    if (sigaction(TIMER_SIGNAL, &sa, NULL) == -1) {
        handle_fatal_error("sigaction");
    }

    // Configure timer
    memset(&sev, 0, sizeof(sev));
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = TIMER_SIGNAL;
    sev.sigev_value.sival_ptr = &timer_id;
    if (timer_create(CLOCK_REALTIME, &sev, &timer_id) == -1) {
        handle_fatal_error("timer_create");
    }

    // Set timer interval
    its.it_value.tv_sec = interval_sec;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = interval_sec;
    its.it_interval.tv_nsec = 0;
    if (timer_settime(timer_id, 0, &its, NULL) == -1) {
        handle_fatal_error("timer_settime");
    }

    printf("[TIMER] Timer initialized with %d-second interval.\n", interval_sec);
}
