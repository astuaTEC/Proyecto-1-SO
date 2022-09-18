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

#define SEM_NAME_1 "/sem_1"
#define SEM_NAME_2 "/sem_2"

#define SHM_SEMS "sems_shared_memory"

typedef struct
{
    int value;
    int index;
    char date[25];
} pixelInfo;


int main(){

    sem_t *sem1 = NULL, *sem2 = NULL;
    pixelInfo *pixels;

    sem_unlink(SEM_NAME_1);
    sem_unlink(SEM_NAME_2);
    shm_unlink(SHM_SEMS);

    sem1 = sem_open(SEM_NAME_1, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);
    sem2 = sem_open(SEM_NAME_2, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);

    int fd_shm = shm_open(SHM_SEMS, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);

    ftruncate(fd_shm, sizeof(pixelInfo)*5); // Array de structs tamaño 5

    pixels = mmap(NULL, sizeof(pixelInfo)*5, PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);

    int i;
    time_t t;   // not a primitive datatype
    time(&t);
	for(i=0; i<5; i++){
        pixels[i].value = i*3;
        strcpy(pixels[i].date, ctime(&t));
    }
    	

    for(i=0; i<5; i += 2) {
        sem_wait(sem2);
        printf("Proceso 1: %d\n", pixels[i].value);
        sleep(1);
        sem_post(sem1);
    }

    sem_close(sem1);
    sem_close(sem2);

    sem_unlink(SEM_NAME_1);
    sem_unlink(SEM_NAME_2);

    munmap(pixels, sizeof(pixelInfo)*5);

    shm_unlink(SHM_SEMS);

    return 0;
}
