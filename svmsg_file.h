#include <stdio.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <stddef.h>                     /* For definition of offsetof() */
#include <limits.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include "tlpi_hdr.h"




#define MSG_SIZE 8192

struct Msg {                    /* Responses (server to client) */
    long mtype;                         /* One of RESP_MT_* values below */
    char data[MSG_SIZE];           /* File content / response message */
};

