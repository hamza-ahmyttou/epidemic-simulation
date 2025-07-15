CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -pedantic -D_XOPEN_SOURCE=700 -I$(INCLUDE_PATH) -c
LDFLAGS=-lpthread -lrt
INCLUDE_PATH=./include
SRC_PATH=./src
BUILD_PATH=./build
EXEC=citizen_manager press_agency

# Object files per executable
CITIZEN_OBJS=$(SRC_PATH)/citizen_manager.o $(SRC_PATH)/timer.o $(SRC_PATH)/epidemic_sim.o
PRESS_OBJS=$(SRC_PATH)/press_agency.o $(SRC_PATH)/timer.o $(SRC_PATH)/epidemic_sim.o

all: $(EXEC)

citizen_manager: $(CITIZEN_OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

press_agency: $(PRESS_OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

$(SRC_PATH)/%.o: $(SRC_PATH)/%.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(SRC_PATH)/*.o
	rm -f $(EXEC)
	rm -f *~
