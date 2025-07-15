#ifndef EPIDEMIC_SIM_H
#define EPIDEMIC_SIM_H

#define CITY_WIDTH 20
#define CITY_HEIGHT 20
#define CITY_POPULATION 100

#include <pthread.h>
#include <stdbool.h>

typedef struct {
    int contamination_level;
    pthread_mutex_t lock;
} Cell;

typedef enum {
    HEALTHY,
    CONTAMINATED,
    RECOVERED,
    DEAD
} HealthState;

typedef struct {
    int id;
    int x, y;
    HealthState state;
    int contamination_level;
    int is_journalist;
} Citizen;

typedef struct {
    int width;
    int height;
    Citizen citizens[CITY_POPULATION];
    int total_population;
    int contaminated_count;
    int dead_count;
    double avg_contamination;
    Cell cells[CITY_HEIGHT][CITY_WIDTH];
} City;

void init_city(City *c);
void simulate_step(City *c);
void update_statistics(City *c);

#endif
