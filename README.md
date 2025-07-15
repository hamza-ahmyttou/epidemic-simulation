
# Epidemic Simulation System

This project simulates an epidemic scenario involving citizens, a press agency, and a timer. It models the behavior of individuals and external systems during an outbreak, with support for concurrency and interprocess communication.

## Project Structure

```
epidemic-sim/
├── include/               # Header files
│   ├── epidemic_sim.h
│   ├── citizen_manager.h
│   ├── press_agency.h
│   └── timer.h
├── src/                   # Source files
│   ├── epidemic_sim.c
│   ├── citizen_manager.c
│   ├── press_agency.c
│   └── timer.c
├── Makefile               # Build configuration
└── README.md              # Project documentation
```

## Components

- **`epidemic_sim`**: Core simulation logic.
- **`citizen_manager`**: Manages behavior and status of simulated citizens.
- **`press_agency`**: Simulates external reporting or communication.
- **`timer`**: Synchronizes events or triggers time-based actions.

## Build Instructions

Ensure you have a C compiler and Make installed. Then run:

```bash
make all
````

This compiles all components: `citizen_manager`, and `press_agency`.

To clean build artifacts:

```bash
make clean
```

## Running

Each executable can be run individually (depending on the simulation design):

```bash
./citizen_manager
./press_agency
```

Make sure any required IPC mechanisms (shared memory, semaphores, etc.) are set up properly before running.

## Dependencies

* POSIX-compliant system (Linux)
* GCC (tested with gcc 13+)
* `pthread`, `rt` libraries
* `usleep()` requires `<unistd.h>`

## Notes

* Avoid duplicating `main()` or common helper functions across modules.
* Shared logic like `handle_fatal_error()` should live in a separate `error_utils.c` to prevent linker errors.

## Troubleshooting

* **Multiple definition errors**: Ensure `handle_fatal_error()` and other helpers are only defined once and shared via headers.
* **`usleep` implicit declaration**: Add `#include <unistd.h>`.
* **Undefined reference to `main`**: Make sure each final executable has a `main()` function.
