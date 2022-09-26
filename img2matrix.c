#define STB_IMAGE_IMPLEMENTATION
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <gsl/gsl_sf.h>
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION

// https://cboard.cprogramming.com/c-programming/178564-loading-image-matrix.html
 
void slice_image(unsigned char *from_image, unsigned char *to_image, int width,
                 int height, int original_channels, int target_channel);
void image2matrix(gsl_matrix * matrix, unsigned char *image, int width,
                  int height)
{
  for (int j = 0; j < height; j++) {
    for (int k = 0; k < width; k++) {
      matrix->data[j * matrix->tda + k] = image[j * width + k];
    }
  }
 
};
 
int main()
{
  printf("Starting\n");
 
  // Load Image
  int width, height, channels;
 
  const char path[] =
      "./imgs/img1-bw.jpg";
  unsigned char *image = stbi_load(path,
                                   &width,
                                   &height,
                                   &channels,
                                   STBI_rgb);
 
  if (image == NULL) {
    printf("Error in loading the image\n");
    exit(1);
  } else {
    printf("Loaded Image\n");
  }
 
 
  printf("Image Params:\n");
  printf("Height: %d ; Width: %d ; Channels: %d\n", width, height, channels);
  // Slice Image
  unsigned char *image_r = malloc(sizeof(int) * height * width);
  if (image_r == NULL) {
    printf("Error allocating space for the image slice\n");
    exit(1);
  }
  printf("Slice Image:\n");
 
  slice_image(image, image_r, width, height, channels, 0);
 
  printf("Allocate Matrix:\n");
  gsl_matrix *matrix = gsl_matrix_alloc(height, width);
  if (matrix == NULL) {
    printf("Error allocating space for the image slice\n");
    exit(1);
  }
  printf("Image to Matrix:\n");
  image2matrix(matrix, image_r, width, height);

  char *filename = "img.txt";
  FILE *fp = fopen(filename, "w");
  print_matrix(fp, matrix);
 
  // FREE ALL
  printf("Free All:\n");
  gsl_matrix_free(matrix);
  free(image_r);
  stbi_image_free(image);
  printf("Finished\n");
 
  return 0;
}
 
void slice_image(unsigned char *from_image, unsigned char *to_image, int width,
                 int height, int original_channels, int target_channel)
{
  unsigned char *p;
  for (int j = 0; j < height; j++) {
    for (int k = 0; k < width; k++) {
      p = &from_image[original_channels * (j * width + k)];
      to_image[(j * width + k)] = p[target_channel];
 
      //printf("%u\n", p[target_channel]);
    }
    //printf("\n");
  }
};
 
int print_matrix(FILE * f, const gsl_matrix * m)
{
  int status, n = 0;
 
  for (size_t i = 0; i < m->size1; i++) {
    for (size_t j = 0; j < m->size2; j++) {
      if ((status = fprintf(f, "%g ", gsl_matrix_get(m, i, j))) < 0)
        return -1;
      n += status;
    }
 
    if ((status = fprintf(f, "\n")) < 0)
      return -1;
    n += status;
  }
 
  return n;
 
}