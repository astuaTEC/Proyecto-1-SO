#include <stdio.h>
#include <stdlib.h>

// For semaphore
#include <semaphore.h>

// Shared memory
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

// FOR FORK
#include <sys/types.h>
#include <unistd.h>

// For killing child process
#include <sys/wait.h>

#define SHM_SEMS "sems_shared_memory"

typedef struct
{
    int shared_var;
    sem_t sem1;
    sem_t sem2;
} Sems;


int main(){

    int fd_shm = shm_open(SHM_SEMS, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);

    ftruncate(fd_shm, sizeof(Sems));

    Sems *sems = mmap(NULL, sizeof(Sems), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);

    sems->shared_var = 0;

    // sem, 1 to share between processes, Initial value of the sem
    sem_init(&sems->sem1, 1, 0);
    sem_init(&sems->sem2, 1, 0);

    if (fork() == 0){ //child
        printf("Child: %d\n", sems->shared_var);
        printf("CHILD: Voy a cambiar el valor a 1\n");
        sems->shared_var = 1;
        printf("Child: %d\n", sems->shared_var);
        sem_post(&sems->sem1);

        munmap(sems, sizeof(Sems));

    } else { //father
        printf("Father: %d\n", sems->shared_var);
        sem_wait(&sems->sem1);
        printf("Father: %d\n", sems->shared_var);
        sem_post(&sems->sem1);

        wait(NULL);

        sem_destroy(&sems->sem1);
        sem_destroy(&sems->sem2);

        munmap(sems, sizeof(Sems));

        shm_unlink(SHM_SEMS);
    }

    return 0;
}