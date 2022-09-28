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

char myImg[20];


// convert grayscale to rgb
// by tripling the values and adjusting the stride to a multiple of 4
char *bw_to_rgb(const guchar *bw, int rows, int cols, int *stride) {
  int r, c, i, stride_adjust;
  guchar *rgb;
 
  *stride = cols * BYTES_PER_PIXEL;
  stride_adjust = (4 - *stride % 4) % 4;
  *stride += stride_adjust;
 
  rgb = malloc(*stride * rows * BYTES_PER_PIXEL);
  for (r = 0; r < rows; r++) {
    for (c = 0; c < cols; c++)
      for (i = 0; i < BYTES_PER_PIXEL; i++)
        rgb[r * *stride + c * BYTES_PER_PIXEL + i] = bw[r * cols + c];
    for (i = 0; i < stride_adjust; i++)
      rgb[r * *stride + cols * BYTES_PER_PIXEL + i] = 0;
  }
 
  return rgb;
}
 
void free_rgb(guchar *pixels, gpointer data) {
  free(pixels);
}

int main(int argc, char *argv[])
{
    if(argc < 2){
        perror("Missing arguments");
        return 1;
    }

    /////////////////////// GUI ///////////////////

    GtkWidget *window, *image;
    GdkPixbuf *pb;
    guchar bw[ROWS * COLS] = {0};
    guchar *rgb;
    int r, c, stride;
    
    // for (r = 0; r < ROWS; r++)
    //     for (c = 0; c < COLS; c++)
    //     bw[r*COLS + c] = rand() % 256;
    

    ////////////////////////////////////

    bzero(myImg, 20);

    int key = atoi(argv[1]);

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

    int i = 0;
    while (1)
    {
        sem_wait(llenos); // down a un lleno
        if( strlen(myImg) == 0 && pixels[i].initPixel == 1){
            strcpy(myImg, pixels[i].imgName);
        }
        if ( strcmp(myImg, pixels[i].imgName) == 0){ //strings are equal
            printf("Decoder: Leo un valor\n");
            printf("Date: %s", pixels[i].date);
            printf("Value: %d\n", pixels[i].value ^ key);
            printf("Index: %d\n", pixels[i].index);
            printf("Img Name: %s\n", pixels[i].imgName);
            printf("-----------------------------\n");
            r = pixels[i].row;
            c = pixels[i].col;
            bw[r*COLS + c] = pixels[i].value ^ key;
            sem_post(huecos); // up a un hueco
        } else {
            sem_post(llenos); // up a llenos si la imagen no corresponde con la que tiene que leer
        }
        if(pixels[i].finalPixel == 1){ //verify the end of the image
            break;
        }
        if( i == length - 1){
            i = -1;
        }
        i++;
        usleep(500);
    }

    rgb = bw_to_rgb(bw, ROWS, COLS, &stride);
    
    gtk_init(&argc, &argv);
 
    pb = gdk_pixbuf_new_from_data(
        rgb,
        GDK_COLORSPACE_RGB,     // colorspace (must be RGB)
        0,                      // has_alpha (0 for no alpha)
        8,                      // bits-per-sample (must be 8)
        COLS, ROWS,             // cols, rows
        stride,                 // rowstride
        free_rgb,               // destroy_fn
        NULL                    // destroy_fn_data
    );
    image = gtk_image_new_from_pixbuf(pb);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Image");
    gtk_window_set_default_size(GTK_WINDOW(window), COLS+20, ROWS+20);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_container_add(GTK_CONTAINER(window), image);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(window);
    
    gtk_main();

    wait(NULL);

    munmap(pixels, sizeof(pixelInfo)*5);

    return 0;
}
