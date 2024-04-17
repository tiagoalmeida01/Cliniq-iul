//92985_tiago_almeida
#include "general.h"

int id_msgQ, auxReceived;
message m;

void connect_to_msgQ()
{
    id_msgQ = msgget(IPC_KEY, 0);
    exit_on_error(id_msgQ, "ERROR! Server not found!\n");
}

void handler_type2()
{
    printf("Consulta iniciada para o processo «%d»\n", getpid());
}

void handler_type3()
{
    if (auxReceived != 0)
    {
        printf("Consulta concluida para o processo «%d»\n", getpid());
        exit(0);
    }
    else
    {
        printf("ERRO: Não recebi mensagem consulta iniciada!\n");
        exit(0);
    }
}

void handler_type4()
{
    printf("Consulta não é possível para o processo «%d»\n", getpid());
    exit(0);
}

void receive_update()
{
    int status;
    int type;
    status = msgrcv(id_msgQ, &m, sizeof(m.text), getpid(), 0);
    exit_on_error(status, "ERROR while receiving message!\n");

    char msg[BUF_SIZE];
    strcpy(msg, m.text);
    type = atoi(msg);

    switch (type)
    {
    case 2:
        auxReceived = 0;
        handler_type2();
        auxReceived++;
        break;
    case 3:
        handler_type3();
        break;
    case 4:
        handler_type4();
        break;
    }
}

void send_request()
{
    m.tipo = 1;
    sprintf(m.text, "%d;%d;%d;%s", c.status, c.pid_consulta, c.tipo, c.descricao);
    sendMessage(m, id_msgQ);
}

void front_desk()
{
    printf("Por favor indique o tipo de consulta (1 - Normal; 2 - COVID19; 3 - Urgente)\n");
    printf("--> ");
    scanf("%d", &c.tipo);
    while (c.tipo > 3 || c.tipo < 1)
    {
        printf("ERROR: Por favor indique um tipo de consulta válido!\n");
        printf("\n--> ");
        scanf("%d", &c.tipo);
    }
    getchar();
    printf("Por favor indique a descrição da consulta (MAX %d caracteres)\n", 100);
    printf("--> ");
    fgets(c.descricao, 100, stdin);
    printf("\nMarcou uma consulta de tipo %d com a descrição %s\n", c.tipo, c.descricao);

    c.pid_consulta = getpid();
    c.status = 1;
    send_request();
}

void sendMessageType5()
{
    m.tipo = getpid();
    c.status = 5;
    sprintf(m.text, "%d", c.status);
    sendMessage(m, id_msgQ);
}

void handler_sigint()
{
    printf("\nPaciente Cancelou o pedido\n");
    sendMessageType5();
    exit(0);
}

int main()
{
    system("clear");
    connect_to_msgQ();
    printf("\nBem-vindo/a à Cliniq-Iul!\nPor favor insira os dados como são requisitados!\nPara cancelar prima «CTRL» + «C»\n\n");
    front_desk();
    signal(SIGINT, handler_sigint);
    while (1)
    {
        receive_update();
    }
}
