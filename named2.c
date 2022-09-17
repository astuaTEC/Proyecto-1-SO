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

int main()
{
    sem_t *sem1 = NULL, *sem2 = NULL;

    int fd_shm = shm_open(SHM_SEMS, O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);

    sem1 = sem_open(SEM_NAME_1, O_RDWR);
    sem2 = sem_open(SEM_NAME_2, O_RDWR);


    myStruct *s = mmap(NULL, sizeof(s), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);

    sem_wait(sem1);
    printf("Proceso 2: %d\n", s->shared_var);
    s->shared_var = 15;
    sleep(2);
    sem_post(sem2);
    sem_wait(sem1);
    sleep(2);
    printf("Proceso 2: %d\n", s->shared_var);

    wait(NULL);

    sem_close(sem1);
    sem_close(sem2);

    sem_unlink(SEM_NAME_1);
    sem_unlink(SEM_NAME_2);

    munmap(s, sizeof(myStruct));

    return 0;
}
