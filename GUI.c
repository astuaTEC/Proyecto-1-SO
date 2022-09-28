#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>


static GtkWidget *square;
static GtkWidget *square2;

int main(int argc, char **argv) {

    //Basic GTK widgets to design
    GtkWidget *window, *grid, *calculate;

    //Window size parameters
    int W = 100;
    int H = 100;

    //Initialized the main window and set up 
    //https://www.ubuntubuzz.com/2018/11/setup-cgtk-programming-tools-on-ubuntu-for-beginners.html
    //https://github.com/engineer-man/youtube/blob/master/007/007_gtk.c
    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(window, "Image Generation");
    gtk_window_set_default_size(window, W, H);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    //Grind arrange interface elements in columns and rows
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    //gtk_label_new -- element to display the image pixel by pixel

    //Parameters (0, 0, 1, 1)
    /*
        First: Number of row
        Second: Number of column
        Third: weigth
        Fourth: Height
    */

    //About color configuration
    //https://stackoverflow.com/questions/1706550/gtk-modifying-background-color-of-gtkbutton

    square = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(grid), square, 0, 0, 1, 1);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}