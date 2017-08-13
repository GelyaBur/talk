#include "svmsg_file.h"
#include <pthread.h>

static char file_name[] = "/tmp.txt";

static void *input(void *arg)
{
    struct Msg msg;
    int res;
    int my_queue = *((int *) arg);
    for (;;) {
        msg.mtype = 1;
        while (fgets(msg.data, MSG_SIZE, stdin) != NULL) {
            msg.data[strlen(msg.data)-1] = '\0';
            res = msgsnd(my_queue, &msg, MSG_SIZE , 0);
            if (res == -1)
                break;
        }
    }
}

int
main(int argc, char *argv[])
{
    int my_queue, queue, res, numRead;
    struct Msg msg, resp;
    key_t key;
    char path[PATH_MAX];
    char path2[PATH_MAX];
    char c = 'x';
    pthread_t t;

    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s person\n", argv[0]);
    sprintf(path, "%s", strcat(getenv("HOME"), "/tmp.txt"));
    key = ftok(path, c);
    
    my_queue = msgget(key, IPC_CREAT | S_IWUSR | S_IRUSR | S_IROTH | S_IWOTH);
    if (my_queue == -1)
        errExit("msgget - my_queue message queue");

    /* Send message that this person is ready */

    msg.mtype = 1;
    sprintf(msg.data, " is ready");
    if (msgsnd(my_queue, &msg, MSG_SIZE, 0) == -1)
        errExit("msgsnd");

    strcpy(path2, "/home/");
    strcat(path2, argv[1]);
    strcat(path2, file_name);
    queue = msgget(ftok(path2, c), IPC_CREAT | S_IWUSR |  S_IRUSR | S_IWOTH | S_IROTH);
    int msgLen = msgrcv(queue, &resp, MSG_SIZE, 0, 0);
    if (msgLen == -1)
        errExit("msgrcv");

    printf("%s%s\n", argv[1], resp.data);
    memset(&resp, 0, sizeof(resp));

    int s = pthread_create(&t, NULL, input, &my_queue);
    if (s != 0)
        errExitEN(s, "pthread_create");
    for(;;) {
        int msgLen = msgrcv(queue, &resp, MSG_SIZE, 0, 0);
        if (msgLen == -1)
            errExit("msgrcv");
        printf("\x1b[31;1m%s\x1b[0m: \x1b[1m%s\x1b[0m\n", argv[1], resp.data);
        memset(&resp, 0, sizeof(resp));
    }
    s = pthread_join(t, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");
    
    exit(EXIT_SUCCESS);
}
