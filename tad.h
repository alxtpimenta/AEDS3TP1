#define STR_MAX 600

typedef struct dataType
{
    int content;
    bool isOccupied;
} dataType;

typedef struct PageType
{
    bool isActive;
    dataType *Data;
    int age;
    int requests;
} PageType;

typedef struct RAM
{
    int MemorySize;
    int PageSize;
    int Frames;
    PageType *Page;
} RAM;

int randGen(int n);
void clearMemory(RAM *Memory);
void purgeMemory(RAM *Memory);
void replacePage(RAM *Memory, int page, int newData);
void fillMemoryFrames(RAM *Memory, int newData, int *firstEmpty);
int firstEmptyFrame(RAM *Memory);
bool checkForPage(RAM *Memory, int data, int *page, int *location);
void Aging(RAM *Memory);
int oldestPage(RAM *Memory);
bool isMemoryEmpty(RAM *Memory);
int simulateSecondaryMemory(RAM *Memory, int newData);
int leastRequested(RAM *Memory);
bool LFUcheck(RAM *Memory, int requests);
int oldestLFU(RAM *Memory, int requests);
int spatialLocality(RAM *Memory, int newData, int lastData);
