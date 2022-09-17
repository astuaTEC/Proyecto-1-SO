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
    int shared_var;
} myStruct;

int main(){

    sem_t *sem1 = NULL, *sem2 = NULL;

    sem1 = sem_open(SEM_NAME_1, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);
    sem2 = sem_open(SEM_NAME_2, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);

    int fd_shm = shm_open(SHM_SEMS, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);

    ftruncate(fd_shm, sizeof(myStruct));

    myStruct *s = mmap(NULL, sizeof(s), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);

    s->shared_var = 0;

    printf("Proceso 1: %d\n", s->shared_var);
    s->shared_var = 40;
    sem_post(sem1);
    sem_wait(sem2);
    printf("Proceso 1: %d\n", s->shared_var);
    s->shared_var = 12;
    sem_post(sem1);

    sem_close(sem1);
    sem_close(sem2);

    munmap(s, sizeof(myStruct));

    sem_unlink(SEM_NAME_1);
    sem_unlink(SEM_NAME_2);

    munmap(s, sizeof(myStruct));

    shm_unlink(SHM_SEMS);

    return 0;
}
