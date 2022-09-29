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
#include <strings.h>
#include <string.h>

// for GUI
#include <gtk/gtk.h>

// For fork
#include <sys/types.h>
#include <unistd.h>

// For killing the child process
#include <sys/wait.h>

#define SEM_NAME_1 "/llenos"
#define SEM_NAME_2 "/huecos"


#define SHM_SEMS "sems_shared_memory"
#define SHM_STATS "stats_shared_memory"

#define ROWS 100
#define COLS 100
#define BYTES_PER_PIXEL 3

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
    int counter, readCounter;
} statsInfo;

typedef struct {
  GtkImage *image;
  int rows, cols, stride;
} ImageData;

char myImg[20];
int key, length;
sem_t *llenos = NULL, *huecos = NULL;
pixelInfo *pixels;
statsInfo *stats;
int i = 0;

void setrgb(guchar *a, int row, int col, int stride,
            guchar bw)
{
    int p = row * stride + col * BYTES_PER_PIXEL;
    a[p] = bw;
    a[p + 1] = bw;
    a[p + 2] = bw;
}

void free_pixels(guchar *pixelsIn, gpointer data) {
  free(pixelsIn);
}

int update_pic(gpointer data) {
 
    ImageData *id = (ImageData*)data;
    GdkPixbuf *pb = gtk_image_get_pixbuf(id->image);
 
    guchar *g = gdk_pixbuf_get_pixels(pb);
    int r, c, finalPixel, valueDeco;
    
    if (stats->readCounter == length)
    {                       // returns to the beginning of the array
        stats->readCounter = 0; // reset the counter
    } 
    i = stats->readCounter;

    if (strlen(myImg) == 0 && pixels[i].initPixel == 1)
    {
        strcpy(myImg, pixels[i].imgName);
    }
    if (strcmp(myImg, pixels[i].imgName) == 0)
    { // strings are equal
            sem_wait(llenos); // down a un lleno
            valueDeco = (pixels[i].value ^ key);
            printf("Decoder: Leo un valor\n");
            printf("Date: %s", pixels[i].date);
            printf("Value: %d\n", valueDeco);
            printf("Index: %d\n", pixels[i].index);
            printf("Img Name: %s\n", pixels[i].imgName);
            printf("-----------------------------\n");
            r = pixels[i].row;
            c = pixels[i].col;
            finalPixel = pixels[i].finalPixel;
            setrgb(g, r, c, id->stride, valueDeco);
            stats->readCounter = i + 1;
            sem_post(huecos);

            if (finalPixel == 1)
            { // verify the end of the image
                return FALSE;
            }
    }

    gtk_image_set_from_pixbuf(GTK_IMAGE(id->image), pb);

    return TRUE; // continue timer
}

int main(int argc, char *argv[])
{
    if(argc < 2){
        perror("Missing arguments");
        return 1;
    }

    bzero(myImg, 20);

    key = atoi(argv[1]);

    int fd_shm = shm_open(SHM_SEMS, O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
    int stats_shm = shm_open(SHM_STATS, O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);

    llenos = sem_open(SEM_NAME_1, O_RDWR); // llenos
    huecos = sem_open(SEM_NAME_2, O_RDWR); // huecos

    if(llenos == SEM_FAILED || huecos == SEM_FAILED || fd_shm == -1 || stats_shm == -1){
        printf("Please, create a encoder first...\n");
        return -1;
    }

    // Get shared memory size
    struct stat buf;
    fstat(fd_shm, &buf);
    length = (int) (buf.st_size / sizeof(pixelInfo));

    printf("Length: %d\n", length);

    pixels = mmap(NULL, sizeof(pixelInfo)*length, PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
    stats = mmap(NULL, sizeof(statsInfo), PROT_READ | PROT_WRITE, MAP_SHARED, stats_shm, 0);

    /////////////////////// GUI ///////////////////
    gtk_init(&argc, &argv);
    
    ImageData id;
    id.rows = ROWS;
    id.cols = COLS;
    id.stride = COLS * BYTES_PER_PIXEL;
    id.stride += (4 - id.stride % 4) % 4; // ensure multiple of 4
    
    guchar *rgb = calloc(ROWS * id.stride, 1);
    
    GdkPixbuf *pb = gdk_pixbuf_new_from_data(
        rgb,
        GDK_COLORSPACE_RGB,     // colorspace
        0,                      // has_alpha
        8,                      // bits-per-sample
        COLS, ROWS,             // cols, rows
        id.stride,              // rowstride
        free_pixels,            // destroy_fn
        NULL                    // destroy_fn_data
    );
    
    id.image = GTK_IMAGE(gtk_image_new_from_pixbuf(pb));
    
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Image");
    gtk_window_set_default_size(GTK_WINDOW(window), COLS, ROWS);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(id.image));
    
    g_timeout_add(10,            // milliseconds
                  update_pic,   // handler function
                  &id);         // data
    
    gtk_widget_show_all(window);
    gtk_main();
    
    ////////////////////////////////////

    wait(NULL);

    munmap(pixels, sizeof(pixelInfo)*5);

    return 0;
}
