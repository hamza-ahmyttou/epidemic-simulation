#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "epidemic_sim.h"

#define CONTAMINATION_PROBABILITY 0.2
#define RECOVERY_PROBABILITY 0.1
#define DEATH_PROBABILITY 0.05

extern City *city;
pthread_mutex_t city_mutex = PTHREAD_MUTEX_INITIALIZER;

void init_city(City *c) {
    int i;
    for (i = 0; i < CITY_POPULATION; i++) {
        c->citizens[i].id = i;
        c->citizens[i].state = HEALTHY;
        c->citizens[i].contamination_level = 0;
        c->citizens[i].is_journalist = (i % 10 == 0); /* approx 10% journalists */
    }
    c->total_population = CITY_POPULATION;
    c->contaminated_count = 0;
    c->dead_count = 0;
    c->avg_contamination = 0.0;
}

void update_statistics(City *c) {
    int i;
    int contaminated = 0;
    int dead = 0;
    double total_contamination = 0.0;

    for (i = 0; i < CITY_POPULATION; i++) {
        if (c->citizens[i].state == CONTAMINATED) {
            contaminated++;
            total_contamination += c->citizens[i].contamination_level;
        } else if (c->citizens[i].state == DEAD) {
            dead++;
        }
    }

    c->contaminated_count = contaminated;
    c->dead_count = dead;
    c->avg_contamination = (contaminated > 0) ? total_contamination / contaminated : 0.0;
}

void simulate_step(City *c) {
    int i;
    pthread_mutex_lock(&city_mutex);

    for (i = 0; i < CITY_POPULATION; i++) {
        Citizen *citizen = &c->citizens[i];

        if (citizen->state == HEALTHY) {
            if ((rand() / (float)RAND_MAX) < CONTAMINATION_PROBABILITY) {
                citizen->state = CONTAMINATED;
                citizen->contamination_level = (rand() % 100) + 1;
            }
        } else if (citizen->state == CONTAMINATED) {
            float roll = rand() / (float)RAND_MAX;
            if (roll < DEATH_PROBABILITY) {
                citizen->state = DEAD;
                citizen->contamination_level = 0;
            } else if (roll < DEATH_PROBABILITY + RECOVERY_PROBABILITY) {
                citizen->state = RECOVERED;
                citizen->contamination_level = 0;
            } else {
                citizen->contamination_level += rand() % 10;
                if (citizen->contamination_level > 100)
                    citizen->contamination_level = 100;
            }
        }
    }

    update_statistics(c);
    pthread_mutex_unlock(&city_mutex);
}
