#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <mqueue.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "epidemic_sim.h"
#include "timer.h"

#define SHM_NAME "/shared_memory"
#define MQ_NAME "/memory_queue"
#define NUM_CITIZENS 37

City *city;

static void handle_fatal_error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void send_journalist_report() {
    mqd_t mqueue = mq_open(MQ_NAME, O_WRONLY | O_CREAT, 0644, NULL);
    if (mqueue == -1) {
        perror("mq_open failed in journalist");
        return;
    }

    int contaminated = 0;
    int total_contamination = 0;
    int dead = 0;
    int journalists_contaminated = 0;

    for (int i = 0; i < NUM_CITIZENS; i++) {
        Citizen *c = &city->citizens[i];
        if (c->state == CONTAMINATED) {
            contaminated++;
            if (c->is_journalist) journalists_contaminated++;
        }
        if (c->state == DEAD) dead++;
    }

    // Sum all contamination levels
    for (int y = 0; y < city->height; y++) {
        for (int x = 0; x < city->width; x++) {
            total_contamination += city->cells[y][x].contamination_level;
        }
    }

    int avg_contamination = total_contamination / (city->width * city->height);

    char buffer[256];

    snprintf(buffer, sizeof(buffer), "Total contaminated: %d\n", contaminated);
    mq_send(mqueue, buffer, strlen(buffer), 2);

    snprintf(buffer, sizeof(buffer), "Average contamination level: %d\n", avg_contamination);
    mq_send(mqueue, buffer, strlen(buffer), 5);

    snprintf(buffer, sizeof(buffer), "Number of dead: %d\n", dead);
    mq_send(mqueue, buffer, strlen(buffer), 10);

    snprintf(buffer, sizeof(buffer), "Contaminated journalists: %d\n", journalists_contaminated);
    mq_send(mqueue, buffer, strlen(buffer), 1);

    mq_close(mqueue);
}

void *manage_citizen(void *arg) {
    int id = *(int *)arg;
    free(arg);

    Citizen *citizen = &city->citizens[id];

    if (citizen->is_journalist) {
        send_journalist_report();
        return NULL;
    }

    // Simulate non-journalist behavior: randomly move and spread contamination
    for (int step = 0; step < 10; step++) {
        int x = citizen->x;
        int y = citizen->y;

        pthread_mutex_lock(&city->cells[y][x].lock);
        if (citizen->state == CONTAMINATED) {
            city->cells[y][x].contamination_level += 5;
            if (city->cells[y][x].contamination_level > 100)
                city->cells[y][x].contamination_level = 100;
        }
        pthread_mutex_unlock(&city->cells[y][x].lock);

        // Move randomly
        int dx = (rand() % 3) - 1;
        int dy = (rand() % 3) - 1;

        citizen->x = (x + dx + city->width) % city->width;
        citizen->y = (y + dy + city->height) % city->height;

        usleep(100000); // Simulate time passing
    }

    return NULL;
}

void initialize_shared_memory() {
    int shm_fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0666);
    if (shm_fd == -1) handle_fatal_error("shm_open");

    if (ftruncate(shm_fd, sizeof(City)) == -1)
        handle_fatal_error("ftruncate");

    city = mmap(NULL, sizeof(City), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (city == MAP_FAILED)
        handle_fatal_error("mmap");

    close(shm_fd);
}

void initialize_city() {
    city->width = CITY_WIDTH;
    city->height = CITY_HEIGHT;

    srand(time(NULL));

    for (int y = 0; y < CITY_HEIGHT; y++) {
        for (int x = 0; x < CITY_WIDTH; x++) {
            city->cells[y][x].contamination_level = 0;

            pthread_mutexattr_t attr;
            pthread_mutexattr_init(&attr);
            pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
            pthread_mutex_init(&city->cells[y][x].lock, &attr);
            pthread_mutexattr_destroy(&attr);
        }
    }

    for (int i = 0; i < NUM_CITIZENS; i++) {
        city->citizens[i].id = i;
        city->citizens[i].x = rand() % CITY_WIDTH;
        city->citizens[i].y = rand() % CITY_HEIGHT;
        city->citizens[i].state = (rand() % 10 < 2) ? CONTAMINATED : HEALTHY;
        city->citizens[i].is_journalist = (i < 2); // First two are journalists
    }
}

int main() {
    City city;
    initialize_city(&city);

    pthread_t threads[NUM_CITIZENS];

    // Start a thread for each citizen
    for (int i = 0; i < NUM_CITIZENS; i++) {
        int *id = malloc(sizeof(int));
        if (id == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        *id = i;
        if (pthread_create(&threads[i], NULL, manage_citizen, id) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    // Join all threads
    for (int i = 0; i < NUM_CITIZENS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
