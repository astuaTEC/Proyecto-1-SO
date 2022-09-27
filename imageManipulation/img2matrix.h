#ifndef IMG2MATRIX_H
#define IMG2MATRIX_H

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <gsl/gsl_sf.h>
#include "stb_image.h"

void slice_image(unsigned char *from_image, unsigned char *to_image, int width,
                 int height, int original_channels, int target_channel);

int print_matrix(FILE * f, const gsl_matrix * m);

gsl_matrix * getMatrixFromImage(const char *imgName);

#endif