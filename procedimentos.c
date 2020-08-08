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
    //RETORNA UM N�MERO ALEAT�RIO ENTRE 0 E n-1
    return rand()%n;
}

void clearMemory(RAM *Memory)
{
    //ZERA TODOS OS VALORES DAS P�GINAS NOS FRAMES DA MEM�RIA
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
    //DESALOCA TODAS AS P�GINAS
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
    //SUBSTITUI A P�GINA page, PELA P�GINA QUE CONT�M O REGISTRO newData
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
    //INSERE A P�GINA QUE CONT�M O REGISTRO newData NO PRIMEIRO FRAME VAZIO DA MEM�RIA
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
    //DETERMINA O PRIMEIRO FRAME VAZIO DA MEM�RIA
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
    //VERIFICA SE O REGISTRO data EST� NA MEM�RIA
    //CASO ESTEJA, ELE RETORNA TRUE, E ENVIA PELOS PAR�METROS A P�GINA E POSI��O QUE O REGISTRO EST�
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
    //INCREMENTA A IDADE DE TODAS AS P�GINAS ATIVAS NOS FRAMES DA MEM�RIA
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
    //DETERMINA A P�GINA ATIVA MAIS VELHA NOS FRAMES DA MEM�RIA
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
    //DETERMINA SE A MEM�RIA AINDA TEM FRAMES VAZIOS
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
    //SIMULA A PAGINA��O DA MEM�RIA SECUND�RIA
    //A FUN��O RETORNA O PRIMEIRO REGISTRO DA P�GINA ONDE O REGISTRO newData ESTARIA NA MEM�RIA SECUND�RIA
    float result = (floor(newData/Memory->PageSize) * Memory->PageSize);

    return (int)result;
}

int leastRequested(RAM *Memory)
{
    //DETERMINA A P�GINA COM O MENOR N�MERO DE REQUISI��ES
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
    //VERIFICA SE H� OUTRA P�GINA NA MEM�RIA COM O MESMO N�MERO DE REQUISI��ES
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
    //DETERMINA A P�GINA MAIS VELHA COM O N�MERO DE REQUISI��ES IGUAL A requests
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
    //RETORNA A DIST�NCIA ENTRE AS P�GINAS QUE CONT�M OS REGISTROS DE ENTRADA
    int newDataPage = (int)floor(newData/Memory->PageSize);

    int lastDataPage = (int)floor(lastData/Memory->PageSize);

    return newDataPage-lastDataPage;
}
