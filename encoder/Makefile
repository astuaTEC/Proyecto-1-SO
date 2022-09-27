# Crear ejecutable para programa.c
# Compilador
CC = gcc
# Opciones de compilacion
CFLAGS = -Wall -I/usr/local/include

# Opciones de compilacion
CFLAGS2 = -L/usr/local/lib

CFLAGS3 = -lgsl -lgslcblas -lm

# Nombre del ejecutable
TARGET = img2matrix

# Archivos fuente de las librerias
SRCS = ../imageManipulation/img2matrix.c

# Objetos
OBJS = $(SRCS:.c=.o)

# Construir el ejecutable
$(TARGET): link encoder
		$(CC) $(CFLAGS2) img2matrix.o encoder.o $(CFLAGS3) -lrt -o enco.out

# Dependencias
link: $(SRCS)
	$(CC) $(CFLAGS) -c $(SRCS)

encoder: encoder.c
	$(CC) -Wall -c encoder.c -lrt

# Eliminar archivos generados
limpiar:
	rm -f $(TARGET) *.o