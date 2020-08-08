#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int randGen(int n)
{
    //RETORNA UM NÚMERO ALEATÓRIO ENTRE 0 E n-1
    return rand()%n;
}

int main()
{
    FILE *OutputFile = fopen("saidateste.txt","w");
    fclose(OutputFile);
    OutputFile = fopen("saidateste.txt","a");

    if (OutputFile == NULL)
    {
        printf("Erro ao abrir o arquivo de saida!\n");
        return 1;
    }

    int requests = 150;

    int max = 121;
    //INICIAR SEMENTE DE NUMEROS ALEATORIOS
    srand((unsigned)time(NULL));
    int i;

    for(i=0;i<requests;i++)
    {
        fprintf(OutputFile,"%d ",randGen(max));
    }

    fclose(OutputFile);
    return 0;
}
