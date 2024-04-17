//92985_tiago_almeida
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "struct.h"
#include <errno.h>

int recebeu_sighup = 0;
int consulta_ativa = 0;

void escrever_pedido(Consulta c)
{
    FILE *rc;
    rc = fopen("PedidoConsulta.txt", "w");
    fprintf(rc, "%d;%d;%s", c.pid_consulta, c.tipo, c.descricao);
    fclose(rc);
}

int read_PID()
{
    int n = 0;
    FILE *file = fopen("SrvConsultas.pid", "r");
    fscanf(file, "%d", &n);
    fclose(file);
    return n;
}

void send_SIGUSR1()
{
    n = 1;
    kill(read_PID(), SIGUSR1);
    n = 0;
}

void verif_exists()
{
    if (file_exists("PedidoConsulta.txt") == 0) // header file
    {
        escrever_pedido(c);
        send_SIGUSR1();
    }
    else
    {
        printf("ERRO: Servidor ocupado! \nPor favor aguarde 10 segundos para o seu pedido ser colocado!\n");
        alarm(10);
    }
}

void front_desk()
{
    printf("Por favor indique o tipo de consulta (1 - Normal; 2 - COVID19; 3 - Urgente)\n");
    printf("-->");
    scanf("%d", &c.tipo);
    while (c.tipo > 3 || c.tipo < 1)
    {
        printf("ERROR \nPor favor indique um tipo de consulta válido!\n");
        printf("\n-->");
        scanf("%d", &c.tipo);
    }
    limpar_enter();

    printf("Por favor indique a descrição da consulta (MAX %d caracteres)\n", BUF_SIZE - 1);
    printf("-->");
    fgets(c.descricao, BUF_SIZE, stdin);
    printf("\nMarcou uma consulta de tipo %d com a descrição %s\n", c.tipo, c.descricao);

    c.pid_consulta = getpid();
    verif_exists();
}

void handler_sigalrm()
{
    n = 1;
    verif_exists();
    n = 0;
}
void handler_sigusR2()
{
    n = 1;
    printf("Consulta não é possivel para o processo «%d»\n", getpid());
    if (remove("PedidoConsulta.txt") != 0)
        printf("Erro a apagar o ficheiro");
    exit(0);
    n = 0;
}

void handler_sigHUP()
{
    n = 1;
    consulta_ativa = 1;
    printf("Consulta iniciada para o processo «%d»\n", getpid());
    remove("PedidoConsulta.txt");
    recebeu_sighup++;
    n = 0;
}

void handler_sigTERM()
{
    n = 1;
    consulta_ativa = 0;
    if (recebeu_sighup != 0)
    {
        printf("Consulta concluída para o processo «%d»\n", getpid());
        exit(0);
    }
    else
    {
        printf("ERROR Não recebi um sighup!\n");
    }
}

void handler_sigint()
{
    n = 1;
    if (consulta_ativa == 1)
    {
        printf("\nERROR Não é possivel cancelar o pedido pois a sua consulta já começou!\n");
    }
    else
    {
        printf("\nPaciente cancelou o pedido!\n");
        remove("PedidoConsulta.txt");
        exit(0);
    }
    n = 0;
}

int main()
{
    front_desk();

    signal(SIGALRM, handler_sigalrm);

    signal(SIGINT, handler_sigint);

    signal(SIGHUP, handler_sigHUP);

    signal(SIGTERM, handler_sigTERM);

    signal(SIGUSR2, handler_sigusR2);

    while (n == 0)
    {
        pause();
    }
}
