//92985_tiago_almeida
typedef struct
{
    int tipo;            // 1-normal ; 2-covid19 ; 3-urgente
    char descricao[100]; // Descrição da consulta
    int pid_consulta;    // PID do processo que quer fazer a consulta

} Consulta;

Consulta c;
int n = 0;
#define BUF_SIZE 100

int limpar_enter()
{ //sebenta p.87 sempre que existe scanf antes do fgets funciona mal
    while (getchar() != '\n')
        ;
}

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
int file_exists(const char *fname)
{
    FILE *file;
    if ((file = fopen(fname, "r")))
    {
        fclose(file);
        return 1; // existe
    }
    return 0; //nao existe
}