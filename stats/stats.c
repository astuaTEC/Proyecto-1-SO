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
#include <string.h>

// For fork
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

// For killing the child process
#include <sys/wait.h>

// For GUI
#include <gtk/gtk.h>

#define SEM_NAME_1 "/llenos"
#define SEM_NAME_2 "/huecos"

#define SHM_SEMS "sems_shared_memory"
#define SHM_STATS "stats_shared_memory"

//Definir los labels de las estadisiticas
static GtkWidget *memoria_total;   //Memorial Total Utilizada: Estructura, variables compartidas
static GtkWidget *tiempo_semaforos;
static GtkWidget *datos_transferidos;
static GtkWidget *tiempo_kernel;    //Tiempo Kernel: escritura, lectura
static GtkWidget *cantidad_pixeles;

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
    int counter, readCounter, pixelsGT175, encoderData, flagRunnig;
    time_t start, end;
    time_t startK, endK;
    double cpu_time_used;
    double kernelTime;
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

    stats->flagRunnig = 0;

    // FOR STATS
    int pixelsGT175 = stats->pixelsGT175;
    // Get shared memory size
    int memorySize;
    struct stat buf;
    fstat(fd_shm, &buf);
    memorySize = (int) (buf.st_size); //size in bytes

    int encoderData = stats->encoderData;
    double kernelTime = stats->kernelTime / 1000;
    double timeSem = (stats->cpu_time_used - stats->kernelTime) / 1000;

    printf("Seconds: %lf\n", timeSem);
    printf("Kernel: %lf\n", stats->kernelTime/1000);

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
    GtkWindow *window;
    GtkWidget *grid;
    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(window, "Estadísticas");
    gtk_window_set_default_size(window, W, H);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    ////////////////////// Memoria total utilizada /////////////////////////////////
    memoria_total = gtk_label_new("");
    char m_number[6];
    sprintf(m_number, "%d", memorySize);
    char memSize[150];
    bzero(memSize, 150);
    strcat(memSize, "Memoria Total Utilizada: ");
    strcat(memSize, m_number);
    strcat(memSize, " bytes");
    gtk_label_set_text(GTK_LABEL(memoria_total), memSize);
    gtk_label_set_xalign(memoria_total,0.0);
    gtk_widget_set_size_request(memoria_total, 10, 30);
    gtk_grid_attach(GTK_GRID(grid), memoria_total, 0, 1, 1, 1);

    //////////////////// Tiempo por semáforos //////////////////////////////
    tiempo_semaforos = gtk_label_new("");
    char t_number[20];
    sprintf(t_number, "%lf", timeSem);
    char timeSems[150];
    bzero(timeSems, 150);
    strcat(timeSems, "Tiempo total por semáforos: ");
    strcat(timeSems, t_number);
    strcat(timeSems, " segundos");
    gtk_label_set_text(GTK_LABEL(tiempo_semaforos), timeSems);
    gtk_label_set_xalign(tiempo_semaforos,0.0);
    gtk_widget_set_size_request(tiempo_semaforos, 10, 30);
    gtk_grid_attach(GTK_GRID(grid), tiempo_semaforos, 0, 2, 1, 1);

    ///////////////// Datos transferidos /////////////////////////////////////
    datos_transferidos = gtk_label_new("");
    char ed_number[12];
    sprintf(ed_number, "%d", encoderData);
    char encoData[200];
    bzero(encoData, 200);
    strcat(encoData, "Total de datos transferidos por encodificadores: ");
    strcat(encoData, ed_number);
    strcat(encoData, " bytes");
    gtk_label_set_text(GTK_LABEL(datos_transferidos), encoData);
    gtk_label_set_xalign(datos_transferidos,0.0);
    gtk_widget_set_size_request(datos_transferidos, 10, 30);
    gtk_grid_attach(GTK_GRID(grid), datos_transferidos, 0, 3, 1, 1);

    ////////////////////// KERNEL ///////////////////////////////////////////////
    tiempo_kernel = gtk_label_new("");
    char tk_number[20];
    sprintf(tk_number, "%lf", kernelTime);
    char timeKernel[150];
    bzero(timeKernel, 150);
    strcat(timeKernel, "Tiempo total en modo Kernel: ");
    strcat(timeKernel, tk_number);
    strcat(timeKernel, " segundos");
    gtk_label_set_text(GTK_LABEL(tiempo_kernel), timeKernel);
    gtk_label_set_xalign(tiempo_kernel,0.0);
    gtk_widget_set_size_request(tiempo_kernel, 10, 30);
    gtk_grid_attach(GTK_GRID(grid), tiempo_kernel, 0, 4, 1, 1);

    ////////////////// Cantidad de pixeles mayores a 175 /////////////////////
    cantidad_pixeles = gtk_label_new("");
    char p_number[6];
    sprintf(p_number, "%d", pixelsGT175);
    char pixeles[200];
    bzero(pixeles, 200);
    strcat(pixeles, "Cantidad de Pixeles mayores a 175: ");
    strcat(pixeles, p_number);
    gtk_label_set_text(GTK_LABEL(cantidad_pixeles), pixeles);
    gtk_label_set_xalign(cantidad_pixeles,0.0);
    gtk_widget_set_size_request(cantidad_pixeles, 10, 30);
    gtk_grid_attach(GTK_GRID(grid), cantidad_pixeles, 0, 5, 1, 1);

    gtk_widget_show_all(window);
    gtk_main();
    /////////////////////////////////////////////////////////////////

    return 0;
}