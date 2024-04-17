//92985_tiago_almeida
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#define exit_on_error(s,m) if (s<0) {perror(m); exit(1);}
#define exit_on_null(s,m) if (s==NULL) { perror(m); exit(1); }
#define IPC_KEY 0x0a92985 // nao esquecer de mudar!

typedef struct
{
    long tipo;
    char text[BUF_SIZE];
} message;