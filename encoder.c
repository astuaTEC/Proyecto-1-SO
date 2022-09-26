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

#define SEM_NAME_1 "/llenos"
#define SEM_NAME_2 "/huecos"

#define SHM_SEMS "sems_shared_memory"

typedef struct
{
    int value;
    int index;
    char date[25];
} pixelInfo;


int main(){

    sem_t *llenos = NULL, *huecos = NULL;
    pixelInfo *pixels;

    llenos = sem_open(SEM_NAME_1, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0); // llenos
    huecos = sem_open(SEM_NAME_2, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 5); // huecos

    if(llenos == SEM_FAILED || huecos == SEM_FAILED){
        printf("Access the semaphores...\n");
        llenos = sem_open(SEM_NAME_1, O_RDWR); // llenos
        huecos = sem_open(SEM_NAME_2, O_RDWR); // huecos
    }

    int fd_shm = shm_open(SHM_SEMS, O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);

    if(fd_shm == -1){
        printf("Create the shared memory...\n");
        fd_shm = shm_open(SHM_SEMS, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
        ftruncate(fd_shm, sizeof(pixelInfo)*5); // Array de structs tamaño 5
    }

    pixels = mmap(NULL, sizeof(pixelInfo)*5, PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);

    time_t t;   // not a primitive datatype
    int r; //random num
    	
    int limitIterations = 10; // chunk iterations
    int counter = 1; // aux counter to verify iterations

    int i;
    for(i=0; i<5; i++) {
        sem_wait(huecos); // down a un hueco
        printf("Encoder: Escribo un valor\n");
        r = rand() % 256;
        pixels[i].value = r;
        pixels[i].index = i;
        time(&t);
        strcpy(pixels[i].date, ctime(&t));
        sem_post(llenos); // up a un lleno
        if( i == 4 && counter < limitIterations ){ // returns to the beginning of the array
            i = -1;  // reset the counter
            counter++; //update aux counter
        }
        sleep(1);
        
    }

    // sem_close(llenos);
    // sem_close(huecos);

    // sem_unlink(SEM_NAME_1);
    // sem_unlink(SEM_NAME_2);

    munmap(pixels, sizeof(pixelInfo)*5);

    //shm_unlink(SHM_SEMS);

    return 0;
}
