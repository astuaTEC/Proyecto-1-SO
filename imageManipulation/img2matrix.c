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

// https://stackoverflow.com/questions/58544166/converting-2d-array-into-a-greyscale-image-in-c
void save(char* file_name, gsl_matrix * m)
{

    FILE* f = fopen(file_name, "wb");

    int width = m->size2;
    int height = m->size1;

    color tablo_color[255];
    for (int i = 0; i < 256; i++){
        tablo_color[i] = ( (color){ (U8)i,(U8)i,(U8)i,(U8)255 } );//BGRA 32 bit
    }    

    U8 pp[54] = { 'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0 ,
                     40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 32 };
    *(int*)(pp + 2) = 54 + 4 * width * height;  //file size
    *(int*)(pp + 18) = width;
    *(int*)(pp + 22) = height;
    *(int*)(pp + 42) = height * width * 4;      //bitmap size
    fwrite(pp, 1, 54, f);


    for (int i = height - 1; i >= 0; i--)
    {
        for (int j = 0; j < width; j++)
        {
            U8 indis = gsl_matrix_get(m, i, j);
            fwrite(tablo_color+indis, 4, 1, f);
        }
    }

    fclose(f);

}

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

  // char *filename = "img.txt";
  // FILE *fp = fopen(filename, "wb");
  // print_matrix(fp, matrix);

  // save("myImg.jpg", matrix);
 
  // FREE ALL
  printf("Free All:\n");
  //gsl_matrix_free(matrix);
  free(image_r);
  stbi_image_free(image);
  printf("Finished\n");
 
  return matrix;
}
