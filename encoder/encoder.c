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
#include<time.h>

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

typedef struct
{
    int value;
    int index;
    char date[25];
    int finalPixel;
    char imgName[20];
} pixelInfo;


int main(int argc, char *argv[]){

    if(argc < 4){
        perror("Missing arguments");
        return 1;
    }

    char *imgName = argv[1];
    int chunkSize = atoi(argv[2]);
    int key = atoi(argv[3]);

    sem_t *llenos = NULL, *huecos = NULL;
    pixelInfo *pixels;

    llenos = sem_open(SEM_NAME_1, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0); // llenos
    huecos = sem_open(SEM_NAME_2, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, chunkSize); // huecos

    if(llenos == SEM_FAILED || huecos == SEM_FAILED){
        printf("Access the semaphores...\n");
        llenos = sem_open(SEM_NAME_1, O_RDWR); // llenos
        huecos = sem_open(SEM_NAME_2, O_RDWR); // huecos
    }

    int fd_shm = shm_open(SHM_SEMS, O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);

    if(fd_shm == -1){
        printf("Create the shared memory...\n");
        fd_shm = shm_open(SHM_SEMS, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
        ftruncate(fd_shm, sizeof(pixelInfo)*chunkSize); // Array de structs tamaño n
    }

    pixels = mmap(NULL, sizeof(pixelInfo)*chunkSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);

    time_t t;   // not a primitive datatype

    gsl_matrix *matrix = getMatrixFromImage(imgName);

    int i = 0;
    int maxRows = matrix->size1;
    int maxCols = matrix->size2;
    for(int row = 0; row < maxRows; row++){
        for(int col = 0; col < maxCols; col++){
            sem_wait(huecos); // down a un hueco
            printf("Encoder: Escribo un valor\n");
            pixels[i].value = (int) gsl_matrix_get(matrix, row, col) ^ key;
            pixels[i].index = i;
            strcpy(pixels[i].imgName, imgName);
            time(&t);
            strcpy(pixels[i].date, ctime(&t));
            if (row == maxRows - 1 && col == maxCols - 1){ // verify last pixel
                pixels[i].finalPixel = 1;
            }
            else {
                pixels[i].finalPixel = 0;
            }
            sem_post(llenos); // up a un lleno
            if( i == chunkSize - 1){ // returns to the beginning of the array
                i = -1;  // reset the counter
            }
            i++;
            sleep(0.5);
            }
    }

    munmap(pixels, sizeof(pixelInfo)*chunkSize);

    return 0;
}
