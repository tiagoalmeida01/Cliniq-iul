//92985_tiago_almeida
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "struct.h"
#include <sys/wait.h>

#define SIZE_CONSULTAS 10

Consulta lista_consultas[SIZE_CONSULTAS];
int count_1;
int count_2;
int count_3;
int count_lost;
int pid_filho = -1;
int pedido_cancelado = 0;

void initialize_fields()
{
    for (int i = 0; i != SIZE_CONSULTAS; i++)
    {
        lista_consultas[i].tipo = -1;
    }
    count_1 = 0;
    count_2 = 0;
    count_3 = 0;
    count_lost = 0;
}

void registar_PID()
{
    FILE *wp;
    wp = fopen("SrvConsultas.pid", "w");
    fprintf(wp, "%d", getpid());
    fclose(wp);
}

void notify_pedido()
{
    char res[BUF_SIZE];
    FILE *PC;
    pedido_cancelado = 0;

    if ((PC = fopen("PedidoConsulta.txt", "r")) != NULL)
    {
        fscanf(PC, "%[^\n]", res);
        fclose(PC);

        char tipo[BUF_SIZE];
        char pid[BUF_SIZE];

        substring(res, pid, ';', 0);
        c.pid_consulta = atoi(pid);
        substring(res, tipo, ';', 1);
        c.tipo = atoi(tipo);

        substring(res, c.descricao, ';', 2);

        printf("Chegou novo pedido de consulta do tipo %d, descrição «%s» e PID %d\n", c.tipo, c.descricao, c.pid_consulta);
    }
    else
    {
        pedido_cancelado++;
    }
}

int get_first_available_room()
{
    for (int i = 0; i != SIZE_CONSULTAS; i++)
    {
        if (lista_consultas[i].tipo == -1)
            return i;
    }

    printf("Lista de consultas cheia!!\n");
    kill(c.pid_consulta, SIGUSR2);
    count_lost++;

    return -1; // se chegou aqui é porque nao havia salas livres
}
void delete_entry(int room)
{
    for (int i = 0; i != SIZE_CONSULTAS; i++)
    {
        if (i == room)
            lista_consultas[i].tipo = -1;
    }
}

void create_fork(int room)
{

    pid_filho = fork();

    if (pid_filho == 0)
    {
        kill(c.pid_consulta, SIGHUP);
        sleep(10);
        printf("Consulta terminada na sala «%d»", room);
        kill(c.pid_consulta, SIGTERM);
        exit(0);
    }

    wait(NULL);
    delete_entry(room);
}

void inscrever_consulta(int room)
{
    if (pedido_cancelado == 0)
    {
        lista_consultas[room] = c;

        switch (c.tipo)
        {
        case 1:
            count_1++;
            break;
        case 2:
            count_2++;
            break;
        case 3:
            count_3++;
            break;
        }

        printf("Consulta agendada para a sala %d\n", room);
        create_fork(room);
        printf("\n**************************************************************************************\n");
    }
}

void handler_sigusR1()
{
    n = 1;
    notify_pedido();
    int available_room = get_first_available_room();
    if (available_room != -1)
    {
        inscrever_consulta(available_room);
    }

    n = 0;
}
void write_stats()
{
    typedef struct
    {
        int counterLost, counter1, counter2, counter3;
    } counters;

    counters cont;
    counters read;

    int c1AUX = 0;
    int c2AUX = 0;
    int c3AUX = 0;
    int ClAUX = 0;

    if (file_exists("StatsConsultas.dat") == 1)
    {
        FILE *fr = fopen("StatsConsultas.dat", "r");
        fread(&read, sizeof(read), 1, fr);
        c1AUX = read.counter1;
        c2AUX = read.counter2;
        c3AUX = read.counter3;
        ClAUX = read.counterLost;
        fclose(fr);
    }

    FILE *fs = fopen("StatsConsultas.dat", "w");
    cont.counterLost = ClAUX + count_lost;
    cont.counter1 = c1AUX + count_1;
    cont.counter2 = c2AUX + count_2;
    cont.counter3 = c3AUX + count_3;
    fwrite(&cont, sizeof(cont), 1, fs);
    fclose(fs);
}

void handler_sigint()
{
    n = 1;
    printf("\n");
    printf("Servidor terminado\n");
    remove("SrvConsultas.pid");
    write_stats();
    exit(0);
}

int main()
{
    initialize_fields();
    registar_PID();
    signal(SIGUSR1, handler_sigusR1);
    signal(SIGINT, handler_sigint);

    while (n == 0)
    {
        pause();
    }
}
