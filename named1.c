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
    char str[5];
} myStruct;


int main(){

    sem_t *sem1 = NULL, *sem2 = NULL;
    myStruct *s;

    sem_unlink(SEM_NAME_1);
    sem_unlink(SEM_NAME_2);
    shm_unlink(SHM_SEMS);

    sem1 = sem_open(SEM_NAME_1, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);
    sem2 = sem_open(SEM_NAME_2, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);

    int fd_shm = shm_open(SHM_SEMS, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);

    ftruncate(fd_shm, sizeof(myStruct)*5); // Array de structs tamaño 5

    s = mmap(NULL, sizeof(myStruct)*5, PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);

    int i;
	for(i=0; i<5; i++){
        s[i].shared_var = i*3;
        strcpy(s[i].str, "HOLA");
    }
    	

    for(i=0; i<5; i += 2) {
        sem_wait(sem2);
        printf("Proceso 1: %d\n", s[i].shared_var );
        sleep(1);
        sem_post(sem1);
    }

    sem_close(sem1);
    sem_close(sem2);

    sem_unlink(SEM_NAME_1);
    sem_unlink(SEM_NAME_2);

    munmap(s, sizeof(myStruct)*5);

    shm_unlink(SHM_SEMS);

    return 0;
}
