// For semaphores
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

// Shared memory
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>

// For fork
#include <sys/types.h>
#include <unistd.h>

// For killing the child process
#include <sys/wait.h>

// aux functions
#include "../imageManipulation/img2matrix.h"

#define SEM_NAME_1 "/llenos"
#define SEM_NAME_2 "/huecos"

#define SHM_SEMS "sems_shared_memory"
#define SHM_STATS "stats_shared_memory"

typedef struct
{
    int value;
    int index;
    char date[25];
    int finalPixel;
    int initPixel;
    char imgName[20];
    int row;
    int col;
} pixelInfo;

typedef struct {
    int counter, readCounter, pixelsGT175, encoderData, flagRunnig;
    time_t startHuecos, endHuecos;
    time_t startLlenos, endLlenos;
    double huecos_time, llenos_time;
} statsInfo;

int64_t millis()
{
    struct timespec now;
    timespec_get(&now, TIME_UTC);
    return ((int64_t) now.tv_sec) * 1000 + ((int64_t) now.tv_nsec) / 1000000;
}

int main(int argc, char *argv[]){

    if(argc < 5){
        perror("Missing arguments");
        return 1;
    }

    char *imgName = argv[1];
    int chunkSize = atoi(argv[2]);
    int key = atoi(argv[3]);
    int mode = atoi(argv[4]);
    int stepTime;
    if(mode == 0){
        if ( argc < 6) stepTime = 0;
        else stepTime = atoi(argv[5]);
    }

    sem_t *llenos = NULL, *huecos = NULL;
    pixelInfo *pixels;
    statsInfo *stats;

    llenos = sem_open(SEM_NAME_1, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0); // llenos
    huecos = sem_open(SEM_NAME_2, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, chunkSize); // huecos

    if(llenos == SEM_FAILED || huecos == SEM_FAILED){
        printf("Access the semaphores...\n");
        llenos = sem_open(SEM_NAME_1, O_RDWR); // llenos
        huecos = sem_open(SEM_NAME_2, O_RDWR); // huecos
    }

    int fd_shm = shm_open(SHM_SEMS, O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
    int stats_shm = shm_open(SHM_STATS, O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);

    if(fd_shm == -1 || stats_shm == -1){
        printf("Create the shared memory...\n");
        fd_shm = shm_open(SHM_SEMS, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
        stats_shm = shm_open(SHM_STATS, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
        ftruncate(fd_shm, sizeof(pixelInfo)*chunkSize); // Array de structs tamaño n
        ftruncate(stats_shm, sizeof(statsInfo));
    }

    pixels = mmap(NULL, sizeof(pixelInfo)*chunkSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
    stats = mmap(NULL, sizeof(statsInfo), PROT_READ | PROT_WRITE, MAP_SHARED, stats_shm, 0);

    time_t t;   // not a primitive datatype

    gsl_matrix *matrix = getMatrixFromImage(imgName);

    char ch;
    stats->flagRunnig = 1;
    int i = 0, value;
    int maxRows = matrix->size1;
    int maxCols = matrix->size2;
    stepTime *= 1000; // to pass millis to micros
    for(int row = 0; row < maxRows; row++){
        for(int col = 0; col < maxCols; col++){

            if(!stats->flagRunnig) break;

            if( mode == 1){
                printf("Please press Enter: ");
                //read a single character
                ch = fgetc(stdin);
                
                if(ch == 0x0A)
                {
                    printf("ENTER KEY is pressed.\n");
                }
            }

            stats->startHuecos = millis();
            sem_wait(huecos); // down a un hueco
            stats->endHuecos = millis();
            stats->huecos_time += stats->endHuecos - stats->startHuecos;

            printf("Encoder: Escribo un valor\n");
            
            if (stats->counter == chunkSize)
            {                       // returns to the beginning of the array
                stats->counter = 0; // reset the counter
            } 
            i = stats->counter;
    
            value = (int)gsl_matrix_get(matrix, row, col);
            pixels[i].index = i;
            pixels[i].value = value ^ key;

            if (value > 175) stats->pixelsGT175++; // for stats

            pixels[i].row = row;
            pixels[i].col = col;
            strcpy(pixels[i].imgName, imgName);
            time(&t);
            strcpy(pixels[i].date, ctime(&t));
            if (row == maxRows - 1 && col == maxCols - 1)
            { // verify last pixel
                pixels[i].finalPixel = 1;
            }
            else
            {
                pixels[i].finalPixel = 0;
            }
            if (row == 0 && col == 0)
            { // verify first pixel
                pixels[i].initPixel = 1;
            }
            else
            {
                pixels[i].initPixel = 0;
            }

            stats->counter = i + 1;
            stats->encoderData += (int)sizeof(pixelInfo);

            if(!stats->flagRunnig) break;

            sem_post(llenos); // up a un lleno

            usleep(stepTime);
        }
    }

    printf("Please press Enter to exit: ");
    //read a single character
    ch = fgetc(stdin);
    
    munmap(pixels, sizeof(pixelInfo)*chunkSize);
    munmap(stats, sizeof(statsInfo));

    return 0;
}
