#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#define MQ_NAME "/memory_queue"
#define MAX_MSG_SIZE 256
#define MAX_MESSAGES 10

static void handle_fatal_error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(void) {
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    mqd_t mqueue = mq_open(MQ_NAME, O_RDONLY | O_CREAT, 0644, &attr);
    if (mqueue == -1) {
        handle_fatal_error("mq_open");
    }

    char buffer[MAX_MSG_SIZE];
    unsigned int prio;

    printf("Press Agency Report:\n");
    printf("=====================\n");

    for (int i = 0; i < 4; i++) {  // Expecting 4 messages from journalist
        ssize_t bytes_read = mq_receive(mqueue, buffer, MAX_MSG_SIZE, &prio);
        if (bytes_read >= 0) {
            buffer[bytes_read] = '\0';
            printf("[Priority %u] %s", prio, buffer);
        } else {
            perror("mq_receive");
        }
    }

    mq_close(mqueue);
    mq_unlink(MQ_NAME);

    printf("=====================\n");
    printf("End of Report\n");

    return 0;
}
