// gcc color.c `pkg-config --cflags gtk+-3.0 --libs gtk+-3.0` -o img
 
#include <gtk/gtk.h>
#include <stdlib.h>
 
#define ROWS 10
#define COLS 10
#define BYTES_PER_PIXEL 3
 
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
 
int main(int argc, char **argv) {
  GtkWidget *window, *image;
  GdkPixbuf *pb;
  guchar bw[ROWS * COLS] = {0};
  guchar *rgb;
  int r, c, stride;
 
  for (r = 0; r < ROWS; r++)
    for (c = 0; c < COLS; c++)
      //if (r / 20 % 2 && c / 20 % 2)
        bw[r*COLS + c] = rand() % 256;
 
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
 
  return 0;
}