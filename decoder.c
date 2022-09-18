// For semaphores
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

// Shared memory
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/shm.h>

#include <stdlib.h>
#include <stdio.h>

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

int main()
{

    sem_t *sem1 = NULL, *sem2 = NULL;


    int fd_shm = shm_open(SHM_SEMS, O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);

    sem1 = sem_open(SEM_NAME_1, O_RDWR);
    sem2 = sem_open(SEM_NAME_2, O_RDWR);

    // Get shared memory size
    struct stat buf;
    fstat(fd_shm, &buf);
    int length = (int) (buf.st_size / sizeof(pixelInfo));

    printf("Length: %d\n", length);

    pixelInfo *pixels = mmap(NULL, sizeof(pixelInfo)*length, PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);

    int limitIterations = 2; // chunk iterations
    int counter = 1; // aux counter to verify iterations

    int i;
    for (i = 0; i < 5; i++)
    {
        sem_wait(sem1);
        printf("Date: %s\n", pixels[i].date);
        printf("Value: %d\n", pixels[i].value);
        printf("Index: %d\n", pixels[i].index);
        if( i == 4 && counter < limitIterations ){
            i = -1;
            counter++;
        }
        sleep(1);
        sem_post(sem2);
    }

    wait(NULL);

    sem_close(sem1);
    sem_close(sem2);

    sem_unlink(SEM_NAME_1);
    sem_unlink(SEM_NAME_2);

    munmap(pixels, sizeof(pixelInfo)*5);

    return 0;
}
