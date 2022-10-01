#define STB_IMAGE_IMPLEMENTATION
#include "img2matrix.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION

typedef unsigned char U8;
typedef struct { U8 p[4]; } color;

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

void slice_image(unsigned char *from_image, unsigned char *to_image, int width,
                 int height, int original_channels, int target_channel)
{
  unsigned char *p;
  for (int j = 0; j < height; j++) {
    for (int k = 0; k < width; k++) {
      p = &from_image[original_channels * (j * width + k)];
      to_image[(j * width + k)] = p[target_channel];
 
    }
  }
};
 
gsl_matrix * getMatrixFromImage(const char *imgName)
{
  printf("Starting\n");
 
  // Load Image
  int width, height, channels;
 
  char path[50];
  bzero(path, 50);

  strcat(path, "../imgs/");
  strcat(path, imgName);

  printf("%s\n", path);

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
 
  // FREE ALL
  printf("Free All...\n");
  //gsl_matrix_free(matrix);
  free(image_r);
  stbi_image_free(image);
  printf("Finished\n");
 
  return matrix;
}
