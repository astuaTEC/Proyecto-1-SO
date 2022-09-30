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

// For GUI
#include <gtk/gtk.h>

#define SEM_NAME_1 "/llenos"
#define SEM_NAME_2 "/huecos"

#define SHM_SEMS "sems_shared_memory"
#define SHM_STATS "stats_shared_memory"

//Definir los labels de las estadisiticas
static GtkLabel *memoria_total;   //Memorial Total Utilizada: Estructura, variables compartidas
static GtkLabel *tiempo_semaforos;
static GtkLabel *datos_transferidos;
static GtkLabel *tiempo_kernel;    //Tiempo Kernel: escritura, lectura
static GtkLabel *cantidad_pixeles;

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
    int counter, readCounter, pixelsGT175;
} statsInfo;


int main(int argc, char **argv){

    sem_t *llenos = NULL, *huecos = NULL;

    int fd_shm = shm_open(SHM_SEMS, O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
    int stats_shm = shm_open(SHM_STATS, O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);

    llenos = sem_open(SEM_NAME_1, O_RDWR); // llenos
    huecos = sem_open(SEM_NAME_2, O_RDWR); // huecos

    if(llenos == SEM_FAILED || huecos == SEM_FAILED || fd_shm == -1 || stats_shm == -1){
        printf("Shared memory or semaphores weren't created...\n");
        return 0;
    }

    statsInfo *stats = mmap(NULL, sizeof(statsInfo), PROT_READ | PROT_WRITE, MAP_SHARED, stats_shm, 0);

    int pixelsGT175 = stats->pixelsGT175;

    printf("Freeing memory and closing the semaphores...\n");

    sem_close(llenos);
    sem_close(huecos);

    sem_unlink(SEM_NAME_1);
    sem_unlink(SEM_NAME_2);

    shm_unlink(SHM_SEMS);
    shm_unlink(SHM_STATS);

    /////////////////////// GUI ////////////////////////////

    int W = 400;
    int H = 150;

    //Configruacion de la ventana 
    GtkWidget *window, *grid;
    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(window, "Estadísticas");
    gtk_window_set_default_size(window, W, H);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    memoria_total = gtk_label_new("Memoria Total Utilizada:");
    gtk_label_set_xalign(memoria_total,0.0);
    gtk_widget_set_size_request(memoria_total, 10, 30);
    gtk_grid_attach(GTK_GRID(grid), memoria_total, 0, 1, 1, 1);

    tiempo_semaforos = gtk_label_new("Tiempo total por semáforos:");
    gtk_label_set_xalign(tiempo_semaforos,0.0);
    gtk_widget_set_size_request(tiempo_semaforos, 10, 30);
    gtk_grid_attach(GTK_GRID(grid), tiempo_semaforos, 0, 2, 1, 1);

    datos_transferidos = gtk_label_new("Total de datos transferidos por encodificadores:");
    gtk_label_set_xalign(datos_transferidos,0.0);
    gtk_widget_set_size_request(datos_transferidos, 10, 30);
    gtk_grid_attach(GTK_GRID(grid), datos_transferidos, 0, 3, 1, 1);

    tiempo_kernel = gtk_label_new("Tiempo en modo Kernel:");
    gtk_label_set_xalign(tiempo_kernel,0.0);
    gtk_widget_set_size_request(tiempo_kernel, 10, 30);
    gtk_grid_attach(GTK_GRID(grid), tiempo_kernel, 0, 4, 1, 1);

    cantidad_pixeles = gtk_label_new("Cantidad de Pixeles:");
    gtk_label_set_xalign(cantidad_pixeles,0.0);
    gtk_widget_set_size_request(cantidad_pixeles, 10, 30);
    gtk_grid_attach(GTK_GRID(grid), cantidad_pixeles, 0, 5, 1, 1);

    gtk_widget_show_all(window);
    gtk_main();
    /////////////////////////////////////////////////////////////////

    return 0;
}