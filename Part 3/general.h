//92985_tiago_almeida
#define BUF_SIZE 1024
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include "message.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/sem.h>

typedef struct
{
    int tipo;            // 1-normal ; 2-covid19 ; 3-urgente
    char descricao[100]; // Descrição da consulta
    int pid_consulta;    // PID do processo que quer fazer a consulta
    int status;          // 1-pedido, 2-iniciada, 3-terminada, 4-recusada, 5-cancelada

} Consulta;

Consulta c;

void substring(char line[], char final[], char splitter, int index) // funcao dada pelo prof na aula pratica
{
    int i = 0, j = 0, aux = 0;
    for (int i = 0; line[i] != '\0'; i++)
    {
        if (line[i] == splitter)
        {
            aux++;
        }
        else if (aux == index)
        {
            final[j++] = line[i];
        }
    }
    final[j] = '\0';
}
void sendMessage(message msg, int idMSGQUEUE)
{
    int status;
    status = msgsnd(idMSGQUEUE, &msg, sizeof(msg.text), 0);
    exit_on_error(status, "ERROR while writting the message!\n");
}
