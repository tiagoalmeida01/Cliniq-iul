//92985_tiago_almeida
#include "general.h"
#define SIZE_CONSULTAS 10 // numero de consultas possiveis ao mesmo tempo
#define DURATION 10       // duracao de cada consulta


typedef struct // estrutura mantida em memoria com os contadores e lista de consultas
{
    int cLOST, C1, C2, C3;
    Consulta lista_consultas[SIZE_CONSULTAS];
} lista_consultas;

// structs dos semaforos
struct sembuf DOWN = {.sem_op = -1};
struct sembuf UP = {.sem_op = +1};

int id_msgQ, status, id_sMEM, id_sem;
int pid_filho, room;
int n = 1; // variavel de controlo para verificar se paciente cancelou a consulta

lista_consultas *l; // apontador para a zona em memoria onde está a estrutura
message m;

void create_sem()
{
    id_sem = semget(IPC_KEY, 1, IPC_CREAT | 0666);
    exit_on_error(id_sem, "ERROR while creating semaphore\n");

    // inicializa o semáforo a 1
    int Status = semctl(id_sem, 0, SETVAL, 1);
    exit_on_error(Status, "ERROR while initializing semaphore\n");
}

void delete_sem()
{
    if (semctl(id_sem, IPC_RMID, 0) == -1)
    {
        printf("ERROR: Semaphore could not be deleted!\n");
    }
}

void sem_UP()
{
    status = semop(id_sem, &UP, 1);
    //printf("Semaforo a: %d\n", semctl(id_sem, 0, GETVAL));
    exit_on_error(status, "UP");
}
void sem_DOWN()
{
    status = semop(id_sem, &DOWN, 1);
    //printf("Semaforo a: %d\n", semctl(id_sem, 0, GETVAL));
    exit_on_error(status, "DOWN");
}

int checkSHM()
{
    int exists = 0;
    if (shmget(IPC_KEY, sizeof(lista_consultas), IPC_CREAT | 0666 | IPC_EXCL) == -1)
    {
        sem_DOWN();
        exists = -1;
        id_sMEM = shmget(IPC_KEY, sizeof(lista_consultas), IPC_CREAT | 0666);
        l = (lista_consultas *)shmat(id_sMEM, 0, 0);
        exit_on_null(l, "ERROR shmat\n");
        sem_UP();
    }
    return exists;
}

void create_shm()
{
    if (checkSHM() == 0)
    {
        sem_DOWN();
        id_sMEM = shmget(IPC_KEY, sizeof(lista_consultas), IPC_CREAT | 0666);
        exit_on_error(id_sMEM, "ERROR SHMGET")

            l = (lista_consultas *)shmat(id_sMEM, 0, 0);
        exit_on_null(l, "ERROR shmat");

        for (int i = 0; i != SIZE_CONSULTAS; i++)
        {
            l->lista_consultas[i].tipo = -1;
        }
        l->C1 = 0;
        l->C2 = 0;
        l->C3 = 0;
        l->cLOST = 0;
        printf("Memoria partilhada inicializada.\n");
        sem_UP();
    }
}

void create_msgq()
{
    id_msgQ = msgget(IPC_KEY, IPC_CREAT | 0666);
    exit_on_error(id_msgQ, "ERROR while creating message queue\n");
}

void delete_msgq()
{
    if (msgctl(id_msgQ, IPC_RMID, NULL) == -1)
    {
        printf("ERROR: Message Queue could not be deleted!\n");
    }
}

void sendMessageType4()
{
    m.tipo = c.pid_consulta;
    c.status = 4;
    sprintf(m.text, "%d", c.status);
    sendMessage(m, id_msgQ);
}
void sendMessageType2()
{
    m.tipo = c.pid_consulta;
    c.status = 2;
    sprintf(m.text, "%d", c.status);
    sendMessage(m, id_msgQ);
}
void sendMessageType3()
{
    m.tipo = c.pid_consulta;
    c.status = 3;
    sprintf(m.text, "%d", c.status);
    sendMessage(m, id_msgQ);
}

int get_first_available_room()
{
    for (int i = 0; i != SIZE_CONSULTAS; i++)
    {
        if (l->lista_consultas[i].tipo == -1)
        {
            return i;
        }
    }
    printf("                     | Lista de consultas cheia |              \n");
    sendMessageType4();
    l->cLOST++;
    return -1;
}

void delete_entry(int room)
{
    for (int i = 0; i != SIZE_CONSULTAS; i++)
    {
        if (i == room)
        {
            l->lista_consultas[i].tipo = -1;
        }
    }
}
void inscrever_consulta(int room)
{
    l->lista_consultas[room] = c;

    switch (c.tipo)
    {
    case 1:
        l->C1++;
        break;
    case 2:
        l->C2++;
        break;
    case 3:
        l->C3++;
        break;
    }

    printf("                | Consulta agendada para a sala «%d» |              \n", room);
}

void handler_cancelada()
{
    sem_DOWN();
    printf(" | Consulta cancelada pelo utilizador «%d» na sala «%d» | \n", c.pid_consulta, room);
    delete_entry(room);
    sem_UP();
    exit(0);
}
void receive_cancel_request()
{
    status = msgrcv(id_msgQ, &m, sizeof(m.text), c.pid_consulta, 0);
    exit_on_error(status, "ERRO ao receber a mensagem!");

    char msg[BUF_SIZE];
    strcpy(msg, m.text);
    int aux = atoi(msg);

    if (aux == 5)
    {
        kill(pid_filho, SIGUSR1);
    }
}
void handler_sigalrm()
{
    n = 1;
    sem_DOWN();
    printf("                 | Consulta terminada na sala «%d» |              \n", room);
    delete_entry(room);
    sendMessageType3();
    sem_UP();
    exit(0);
}
void createFork()
{
    pid_filho = fork();

    if (pid_filho < 0)
    {
        printf("ERROR FORK!\n");
    }
    else if (pid_filho == 0)
    {
        sleep(1); //tempo de ir para a sala :)
        pid_filho = getpid();
        sem_DOWN();
        room = get_first_available_room();
        if (room != -1)
        {
            inscrever_consulta(room);
            sendMessageType2();
            sem_UP();
            n = 0;
            alarm(DURATION);
            signal(SIGALRM, handler_sigalrm);
        }
        else
        {
            sem_UP();
            exit(0);
        }
    }
    else
    {
        waitpid(-1, NULL, WNOHANG);
    }
}

void handler_type1(char msg[])
{
    char tipo[BUF_SIZE];
    char pid[BUF_SIZE];
    substring(msg, pid, ';', 1);
    substring(msg, tipo, ';', 2);
    substring(msg, c.descricao, ';', 3);
    c.pid_consulta = atoi(pid);
    c.tipo = atoi(tipo);
    c.descricao[strcspn(c.descricao, "\n")] = 0; // remove \n da string

    printf("\n | Chegou novo pedido de consulta do tipo «%d», descrição «%s» e PID «%d» | \n", c.tipo, c.descricao, c.pid_consulta);

    createFork();
}

void receive_request_mailbox1()
{
    status = msgrcv(id_msgQ, &m, sizeof(m.text), 1, 0);
    exit_on_error(status, "ERRO ao receber a mensagem!");

    char msg[BUF_SIZE];
    char statusMQ[BUF_SIZE];
    strcpy(msg, m.text);
    substring(msg, statusMQ, ';', 0);
    c.status = atoi(statusMQ);

    if (c.status == 1)
    {
        handler_type1(msg);
    }
}

void handler_sigint()
{
    delete_msgq();
    delete_sem();
    printf("\n********************* | Servidor terminado | ************************\n");
    printf("\n******************** | Estatisticas Consultas | *********************\n");
    printf("                       | Consultas Normais: %d  |\n", l->C1);
    printf("                       | Consultas COVID-19: %d |\n", l->C2);
    printf("                       | Consultas Urgentes: %d |\n", l->C3);
    printf("                       | Consultas Perdidas: %d |\n", l->cLOST);
    printf("\n*************************** | CLINIQ-IUL | **************************\n");
    exit(0);
}

void aux()
{
    printf("ID Shared Memory: %d\n", id_sMEM);
    printf("ID Message Queue: %d\n", id_msgQ);
    printf("ID Semáforo: %d\n", id_sem);
}

int main()
{
    system("clear");
    printf("\n*************************** | CLINIQ-IUL | **************************\n");
    printf("\n********************* | Servidor inicializado | *********************\n");
    printf("            | Duração Consulta: %d || Numero de Salas: %d | \n", DURATION, SIZE_CONSULTAS);

    create_sem();
    create_msgq();
    create_shm();

    aux();

    signal(SIGINT, handler_sigint);

    while (1)
    {
        receive_request_mailbox1();

        signal(SIGUSR1, handler_cancelada);

        if (n == 0)
        {
            receive_cancel_request();
        }
    }
}
