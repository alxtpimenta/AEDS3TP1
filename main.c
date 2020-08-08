#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include "tad.h"

int main(int argc, char *argv[])
{
    FILE *InputFile = fopen(argv[1],"r");

    if (InputFile == NULL)
    {
        printf("Erro ao abrir o arquivo de entrada!\n");
        return 1;
    }

    FILE *OutputFile = fopen(argv[2],"w");
    fclose(OutputFile);
    OutputFile = fopen(argv[2],"a");

    if (OutputFile == NULL)
    {
        printf("Erro ao abrir o arquivo de saida!\n");
        return 2;
    }
    //INICIAR SEMENTE DE NUMEROS ALEATORIOS
    srand((unsigned)time(NULL));

    int Instances,requests,i,y,k,aux=0,aux2=0;

    //CONTADORES DE PAGE FAULTS
    int FIFOPageFaults;
    int LRUPageFaults;
    int LFUPageFaults;
    int RANDPageFaults;

    //VARIÁVEL DA MEMÓRIA VIRTUAL PRINCIPAL
    RAM MainMemory;

    //FILA RE REQUISIÇÕES
    int *requestQueue;

    //VETOR PARA ARMAZENAR LOCALIDADE DE REFERENCIA TEMPORAL
    int *temporal;
    //VETOR PARA ARMAZENAR LOCALIDADE DE REFERENCIA ESPACIAL
    int *spatial;
    int lastRequest;

    //STRING PARA ARMAZENAR OS DADOS LIDOS DO ARQUIVO DE ENTRADA
    char str[STR_MAX];

    //INÍCIO DA LEITURA DO ARQUIVO
    fgets(str,STR_MAX,InputFile);

    Instances = atoi(str);

    printf("Instances: %d\n",Instances);

    //LOOP DE INSTÂNCIAS
    for(k=0;k<Instances;k++)
    {
        //LEITURA DO TAMANHO DA MEMORIA, TAMANHO DA PAGINA, E NUMERO DE REQUISIÇÕES
        fgets(str,STR_MAX,InputFile);

        MainMemory.MemorySize = atoi(strtok(str," "));
        MainMemory.PageSize = atoi(strtok(NULL," "));
        requests = atoi(strtok(NULL," "));

        //DETERMINAR O NÚMERO DE FRAMES (MOLDURAS DE PÁGINAS) E ALOCAR ESPAÇO PARA A MEMÓRIA VIRTUAL
        MainMemory.Frames = MainMemory.MemorySize/MainMemory.PageSize;
        MainMemory.Page = (PageType*)malloc(MainMemory.Frames * sizeof(PageType));
        for(i=0;i<MainMemory.Frames;i++)
        {
            MainMemory.Page[i].Data = (dataType*)malloc(MainMemory.PageSize * sizeof(dataType));
        }

        printf("--------------Instance %d--------------\n",k+1);
        printf("Memory Size: %d bytes\n",MainMemory.MemorySize);
        printf("Page Size: %d bytes\n",MainMemory.PageSize);
        printf("Frames: %d\n",MainMemory.Frames);
        printf("Requests: %d\n",requests);

        //ALOCAR ESPAÇO PARA A FILA DE REQUISIÇÕES
        requestQueue = (int*)malloc(requests * sizeof(int));
        temporal = (int*)malloc(requests * sizeof(int));
        spatial = (int*)malloc((requests-1) * sizeof(int));

        fgets(str,STR_MAX,InputFile);
        //PREENCHER OS VALORES DA FILA
        for(i=0;i<requests;i++)
        {
            if(i==0)
                requestQueue[i] = atoi(strtok(str," "));
            else
                requestQueue[i] = atoi(strtok(NULL," "));
        }

        clearMemory(&MainMemory);

        //COMEÇO DO FIFO
        FIFOPageFaults = 0;
        printf("---------FIFO---------\n");

        for(y=0;y<requests;y++)
        {

            //VERIFICA SE A PÁGINA JÁ ESTÁ EM MEMÓRIA. CASO ESTEJA, É PAGE HIT
            if(checkForPage(&MainMemory,requestQueue[y],&aux,&aux2))
            {
                printf("Page Hit. Requested: %d\n",requestQueue[y]);
            }
            else
            {
                if(isMemoryEmpty(&MainMemory))
                {
                    //SE AINDA HOUVER ESPAÇO NA MEMÓRIA, A PÁGINA SERÁ INSERIDA NO PRIMEIRO FRAME LIVRE
                    fillMemoryFrames(&MainMemory,requestQueue[y],&aux);
                    FIFOPageFaults ++;
                }
                else
                {
                    //SE A MEMÓRIA ESTIVER CHEIA, A PÁGINA MAIS ANTIGA SERÁ SUBSTITUIDA
                    aux = oldestPage(&MainMemory);
                    replacePage(&MainMemory,aux,requestQueue[y]);
                    FIFOPageFaults++;
                }
                printf("Page Fault. Requested: %d\n",requestQueue[y]);
            }
            Aging(&MainMemory);
        }
        //FIM DO FIFO

        clearMemory(&MainMemory);

        //COMEÇO DO LRU
        LRUPageFaults = 0;
        printf("---------LRU---------\n");
        lastRequest = requestQueue[0];

        for(y=0;y<requests;y++)
        {

            //VERIFICA SE A PÁGINA JÁ ESTÁ EM MEMÓRIA. CASO ESTEJA, É PAGE HIT
            if(checkForPage(&MainMemory,requestQueue[y],&aux,&aux2))
            {
                //AO SER ACESSADA, A IDADE DA PAGINA VOLTA A SER ZERO (LRU)
                MainMemory.Page[aux].age = 0;
                printf("Page Hit. Requested: %d\n",requestQueue[y]);
                //LOCALIDADE DE REFERÊNCIA TEMPORAL
                temporal[y] = aux;
                //LOCALIDADE DE REFERÊNCIA ESPACIAL
                if(y>0)
                {
                    spatial[y-1] = spatialLocality(&MainMemory,requestQueue[y],lastRequest);
                    lastRequest = requestQueue[y];
                }
            }
            else
            {
                if(isMemoryEmpty(&MainMemory))
                {
                    //SE AINDA HOUVER ESPAÇO NA MEMÓRIA, A PÁGINA SERÁ INSERIDA NO PRIMEIRO FRAME LIVRE
                    fillMemoryFrames(&MainMemory,requestQueue[y],&aux);
                    LRUPageFaults ++;
                    //LOCALIDADE DE REFERÊNCIA TEMPORAL
                    temporal[y] = -1;
                    //LOCALIDADE ESPACIAL
                    if(y>0)
                    {
                        spatial[y-1] = spatialLocality(&MainMemory,requestQueue[y],lastRequest);
                        lastRequest = requestQueue[y];
                    }
                }
                else
                {
                    //SE A MEMÓRIA ESTIVER CHEIA, A PÁGINA MAIS ANTIGA SERÁ SUBSTITUIDA
                    aux = oldestPage(&MainMemory);
                    replacePage(&MainMemory,aux,requestQueue[y]);
                    LRUPageFaults++;
                    //LOCALIDADE DE REFERÊNCIA TEMPORAL
                    temporal[y] = -1;
                    //LOCALIDADE ESPACIAL
                    if(y>0)
                    {
                        spatial[y-1] = spatialLocality(&MainMemory,requestQueue[y],lastRequest);
                        lastRequest = requestQueue[y];
                    }
                }
                printf("Page Fault. Requested: %d\n",requestQueue[y]);
            }
            Aging(&MainMemory);
        }
        //FIM DO LRU

        clearMemory(&MainMemory);

        //COMEÇO DO LFU
        LFUPageFaults = 0;
        printf("---------LFU---------\n");

        for(y=0;y<requests;y++)
        {

            //VERIFICA SE A PÁGINA JÁ ESTÁ EM MEMÓRIA. CASO ESTEJA, É PAGE HIT
            if(checkForPage(&MainMemory,requestQueue[y],&aux,&aux2))
            {
                //AO SER ACESSADA, O CONTADOR DE REQUISIÇÕES DA PÁGINA É INCREMENTADO (LFU)
                MainMemory.Page[aux].requests ++;
                printf("Page Hit. Requested: %d\n",requestQueue[y]);
            }
            else
            {
                if(isMemoryEmpty(&MainMemory))
                {
                    //SE AINDA HOUVER ESPAÇO NA MEMÓRIA, A PÁGINA SERÁ INSERIDA NO PRIMEIRO FRAME LIVRE
                    fillMemoryFrames(&MainMemory,requestQueue[y],&aux);
                    LFUPageFaults ++;
                }
                else
                {
                    //SE A MEMÓRIA ESTIVER CHEIA, DETERMINAR PÁGINA COM MENOR NÚMERO DE REQUISIÇÕES (LFU)
                    aux = leastRequested(&MainMemory);
                    //SE HOUVER OUTRA(S) PÁGINA(S) COM O MESMO NÚMERO DE REQUISIÇÕES
                    //A PÁGINA MAIS VELHA COM MESMO NÚMERO DE REQUISIÇÕES SERÁ SUBSTITUIDA (FIFO)
                    if(LFUcheck(&MainMemory,aux))
                    {
                        aux2 = oldestLFU(&MainMemory,aux);
                        replacePage(&MainMemory,aux2,requestQueue[y]);
                        LFUPageFaults++;
                    }
                    else
                    {
                        //CASO NÃO TENHA, A PÁGINA DETERMINADA SERÁ SUBSTITUIDA (LFU)
                        replacePage(&MainMemory,aux,requestQueue[y]);
                        LFUPageFaults++;
                    }
                }
                printf("Page Fault. Requested: %d\n",requestQueue[y]);
            }
            Aging(&MainMemory);
        }
        //FIM DO LFU

        clearMemory(&MainMemory);

        //COMEÇO DO RAND
        RANDPageFaults = 0;
        printf("---------RAND---------\n");

        for(y=0;y<requests;y++)
        {

            //VERIFICA SE A PÁGINA JÁ ESTÁ EM MEMÓRIA. CASO ESTEJA, É PAGE HIT
            if(checkForPage(&MainMemory,requestQueue[y],&aux,&aux2))
            {
                printf("Page Hit. Requested: %d\n",requestQueue[y]);
            }
            else
            {
                if(isMemoryEmpty(&MainMemory))
                {
                    //SE AINDA HOUVER ESPAÇO NA MEMÓRIA, A PÁGINA SERÁ INSERIDA NO PRIMEIRO FRAME LIVRE
                    fillMemoryFrames(&MainMemory,requestQueue[y],&aux);
                    RANDPageFaults ++;
                }
                else
                {
                    //SE NÃO HOUVER ESPAÇO NA MEMÓRIA, UMA PÁGINA DA MEMÓRIA SERÁ SORTEADA ALEATÓRIAMENTE PARA SUBSTITUIÇÃO (RAND)
                    aux = randGen(MainMemory.Frames);
                    replacePage(&MainMemory,aux,requestQueue[y]);
                    RANDPageFaults++;
                }
                printf("Page Fault. Requested: %d\n",requestQueue[y]);
            }
        }
        //FIM DO RAND

        clearMemory(&MainMemory);

        //REFERÊNCIA ESPACIAL E TEMPORAL (TO-DO)

        //IMPRESSÃO
        printf("------Page Faults------\nFIFO: %d\nLRU: %d\nLFU: %d\nRAND: %d\n",FIFOPageFaults,LRUPageFaults,LFUPageFaults,RANDPageFaults);
        fprintf(OutputFile,"%d %d %d %d\n",FIFOPageFaults,LRUPageFaults,LFUPageFaults,RANDPageFaults);
        printf("------Spatial Locality------\n");
        for(i=0;i<requests-1;i++)
        {
            printf("%d ",spatial[i]);
            fprintf(OutputFile,"%d ",spatial[i]);
        }
        printf("\n");
        fprintf(OutputFile,"\n");
        printf("------Temporal Locality------\n");
        for(i=0;i<requests;i++)
        {
            printf("%d ",temporal[i]);
            fprintf(OutputFile,"%d ",temporal[i]);
        }
        printf("\n");
        fprintf(OutputFile,"\n");

        //DESALOCAR MEMÓRIA PARA PROXIMA INSTÂNCIA OU TÉRMINO DA EXECUÇÃO
        free(requestQueue);
        free(temporal);
        free(spatial);
        purgeMemory(&MainMemory);
    }
    fclose(InputFile);
    fclose(OutputFile);
    return 0;
}
