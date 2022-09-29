#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>

//Definir los labels de las estadisiticas
static GtkWidget *memoria_total;   //Memorial Total Utilizada: Estructura, variables compartidas
static GtkWidget *tiempo_semaforos;
static GtkWidget *datos_transferidos;
static GtkWidget *tiempo_kernel;    //Tiempo Kernel: escritura, lectura
static GtkWidget *cantidad_pixeles;

// gcc 007_gtk.c -o 007_gtk `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0`
int main(int argc, char **argv) {


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

    return 0;
}