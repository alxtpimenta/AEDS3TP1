#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include "tad.h"

int randGen(int n)
{
    //RETORNA UM NÚMERO ALEATÓRIO ENTRE 0 E n-1
    return rand()%n;
}

void clearMemory(RAM *Memory)
{
    //ZERA TODOS OS VALORES DAS PÁGINAS NOS FRAMES DA MEMÓRIA
    int i,y;
    for(i=0;i<Memory->Frames;i++)
    {
        Memory->Page[i].isActive = false;
        Memory->Page[i].age = NULL;
        Memory->Page[i].requests = NULL;
        for(y=0;y<Memory->PageSize;y++)
        {
            Memory->Page[i].Data[y].content = NULL;
            Memory->Page[i].Data[y].isOccupied = false;
        }

    }
    return;
}

void purgeMemory(RAM *Memory)
{
    //DESALOCA TODAS AS PÁGINAS
    int i;
    for(i=0;i<Memory->Frames;i++)
    {
        free(Memory->Page[i].Data);
    }
    free(Memory->Page);
    Memory->MemorySize = NULL;
    Memory->PageSize = NULL;
    Memory->Frames = NULL;
    return;
}

void replacePage(RAM *Memory, int page, int newData)
{
    //SUBSTITUI A PÁGINA page, PELA PÁGINA QUE CONTÉM O REGISTRO newData
    int i,aux;

    Memory->Page[page].age = 0;
    Memory->Page[page].isActive = true;

    aux = simulateSecondaryMemory(Memory,newData);
    for(i=0;i<Memory->PageSize;i++)
    {
        Memory->Page[page].Data[i].content = aux;
        Memory->Page[page].Data[i].isOccupied = true;
        aux++;
    }
}

void fillMemoryFrames(RAM *Memory, int newData, int *firstEmpty)
{
    //INSERE A PÁGINA QUE CONTÉM O REGISTRO newData NO PRIMEIRO FRAME VAZIO DA MEMÓRIA
    int i,aux,first;

    first = firstEmptyFrame(Memory);

    *firstEmpty = first;

    Memory->Page[first].age = 0;
    Memory->Page[first].requests = 0;
    Memory->Page[first].isActive = true;

    aux = simulateSecondaryMemory(Memory,newData);
    for(i=0;i<Memory->PageSize;i++)
    {
        Memory->Page[first].Data[i].content = aux;
        Memory->Page[first].Data[i].isOccupied = true;
        aux++;
    }
}

int firstEmptyFrame(RAM *Memory)
{
    //DETERMINA O PRIMEIRO FRAME VAZIO DA MEMÓRIA
    int i;
    for(i=0;i<Memory->Frames;i++)
    {
        if(Memory->Page[i].isActive == false)
            return i;
    }
    return 0;
}

bool checkForPage(RAM *Memory, int data, int *page, int *location)
{
    //VERIFICA SE O REGISTRO data ESTÁ NA MEMÓRIA
    //CASO ESTEJA, ELE RETORNA TRUE, E ENVIA PELOS PARÂMETROS A PÁGINA E POSIÇÃO QUE O REGISTRO ESTÁ
    int i,y;
    for(i=0;i<Memory->Frames;i++)
    {
        for(y=0;y<Memory->PageSize;y++)
        {
            if((Memory->Page[i].isActive) && (Memory->Page[i].Data[y].isOccupied) && (Memory->Page[i].Data[y].content == data))
            {
                *page = i;
                *location = y;
                return true;
            }
        }
    }

    return false;
}

void Aging(RAM *Memory)
{
    //INCREMENTA A IDADE DE TODAS AS PÁGINAS ATIVAS NOS FRAMES DA MEMÓRIA
    int i;
    for(i=0;i<Memory->Frames;i++)
    {
        if(Memory->Page[i].isActive)
            Memory->Page[i].age ++;
    }
    return;
}

int oldestPage(RAM *Memory)
{
    //DETERMINA A PÁGINA ATIVA MAIS VELHA NOS FRAMES DA MEMÓRIA
    int i,oldest=0,location=0;
    for(i=0;i<Memory->Frames;i++)
    {
        if(Memory->Page[i].age > oldest && Memory->Page[i].isActive)
        {
            oldest = Memory->Page[i].age;
            location = i;
        }
    }
    return location;
}

bool isMemoryEmpty(RAM *Memory)
{
    //DETERMINA SE A MEMÓRIA AINDA TEM FRAMES VAZIOS
    int i;
    for(i=0;i<Memory->Frames;i++)
    {
        if(Memory->Page[i].isActive == false)
            return true;
    }
    return false;
}

int simulateSecondaryMemory(RAM *Memory, int newData)
{
    //SIMULA A PAGINAÇÃO DA MEMÓRIA SECUNDÁRIA
    //A FUNÇÃO RETORNA O PRIMEIRO REGISTRO DA PÁGINA ONDE O REGISTRO newData ESTARIA NA MEMÓRIA SECUNDÁRIA
    float result = (floor(newData/Memory->PageSize) * Memory->PageSize);

    return (int)result;
}

int leastRequested(RAM *Memory)
{
    //DETERMINA A PÁGINA COM O MENOR NÚMERO DE REQUISIÇÕES
    int i,location=0;
    int smallest = INT_MAX;
    for(i=0;i<Memory->Frames;i++)
    {
        if(Memory->Page[i].requests < smallest)
        {
            smallest = Memory->Page[i].requests;
            location = i;
        }
    }

    return location;
}

bool LFUcheck(RAM *Memory, int requests)
{
    //VERIFICA SE HÁ OUTRA PÁGINA NA MEMÓRIA COM O MESMO NÚMERO DE REQUISIÇÕES
    int i;
    for(i=0;i<Memory->Frames;i++)
    {
        if(Memory->Page[i].requests == requests)
            return true;
    }
    return false;
}

int oldestLFU(RAM *Memory, int requests)
{
    //DETERMINA A PÁGINA MAIS VELHA COM O NÚMERO DE REQUISIÇÕES IGUAL A requests
    int i,oldest=0,location=0;
    for(i=0;i<Memory->Frames;i++)
    {
        if( (Memory->Page[i].requests == requests) && (Memory->Page[i].age > oldest) && (Memory->Page[i].isActive) )
        {
            oldest = Memory->Page[i].age;
            location = i;
        }
    }

    return location;
}

int spatialLocality(RAM *Memory, int newData, int lastData)
{
    //RETORNA A DISTÂNCIA ENTRE AS PÁGINAS QUE CONTÉM OS REGISTROS DE ENTRADA
    int newDataPage = (int)floor(newData/Memory->PageSize);

    int lastDataPage = (int)floor(lastData/Memory->PageSize);

    return newDataPage-lastDataPage;
}
