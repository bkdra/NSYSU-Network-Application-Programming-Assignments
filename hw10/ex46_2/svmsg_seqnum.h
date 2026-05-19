#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

#define SERVER_KEY 0x1aaaaaa1

struct requestMsg {
    long mtype;                 /* message type, must be > 0 */
    int clientId;              /* private message queue ID */
    int pid;
    int seqLen;                 /* Length of desired sequence */
};

struct responseMsg {
    long mtype;                 /* message type, must be > 0 */
    int seqNum;                /* First number in allocated sequence */
};