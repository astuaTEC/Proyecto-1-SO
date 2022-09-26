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

#define SEM_NAME_1 "/llenos"
#define SEM_NAME_2 "/huecos"


#define SHM_SEMS "sems_shared_memory"

typedef struct
{
    int value;
    int index;
    char date[25];
} pixelInfo;

int main()
{

    sem_t *llenos = NULL, *huecos = NULL;


    int fd_shm = shm_open(SHM_SEMS, O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);

    llenos = sem_open(SEM_NAME_1, O_RDWR); // llenos
    huecos = sem_open(SEM_NAME_2, O_RDWR); // huecos

    if(llenos == SEM_FAILED || huecos == SEM_FAILED || fd_shm == -1){
        printf("Please, create a encoder first...\n");
        return -1;
    }

    // Get shared memory size
    struct stat buf;
    fstat(fd_shm, &buf);
    int length = (int) (buf.st_size / sizeof(pixelInfo));

    printf("Length: %d\n", length);

    pixelInfo *pixels = mmap(NULL, sizeof(pixelInfo)*length, PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);

    int limitIterations = 10; // chunk iterations
    int counter = 1; // aux counter to verify iterations

    int i;
    for (i = 0; i < 5; i++)
    {
        sem_wait(llenos); // down a un lleno
        printf("Decoder: Leo un valor\n");
        printf("Date: %s", pixels[i].date);
        printf("Value: %d\n", pixels[i].value);
        printf("Index: %d\n", pixels[i].index);
        printf("-----------------------------\n");
        sem_post(huecos); // up a un hueco
        if( i == 4 && counter < limitIterations ){
            i = -1;
            counter++;
        }
        sleep(1);
    }

    wait(NULL);

    // sem_close(llenos);
    // sem_close(huecos);

    // sem_unlink(SEM_NAME_1);
    // sem_unlink(SEM_NAME_2);

    munmap(pixels, sizeof(pixelInfo)*5);

    return 0;
}
