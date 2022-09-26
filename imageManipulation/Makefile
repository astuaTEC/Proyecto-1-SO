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
SRCS = img2matrix.c

# Objetos
OBJS = $(SRCS:.c=.o)

# Construir el ejecutable
$(TARGET): link
		$(CC) $(CFLAGS2) $(OBJS) $(CFLAGS3) -o $(TARGET).out

# Dependencias
link: $(SRCS)
	$(CC) $(CFLAGS) -c $(SRCS)

# Eliminar archivos generados
limpiar:
	rm -f $(TARGET) *.o