# Definiciones de variables
CC = g++
CFLAGS = -Wall -Wextra -Iinclude
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
TARGET = $(BIN_DIR)/ejercicio3

# Archivos fuente
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
# Archivos objeto
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

# Regla por defecto
all: $(TARGET)

# Regla para enlazar el ejecutable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# Regla para compilar los archivos fuente
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Regla para limpiar los archivos generados
clean:
	rm -f $(OBJ_DIR)/*.o $(TARGET)

# Regla para mostrar la ayuda
help:
	@echo "Uso: $(TARGET) -h|--help"

# Regla para ejecutar el programa con argumentos
run: 
	@echo "Uso: $(TARGET) -l|--log [Archivo log] -i|--ids [Archivo de IDs validos]"

.PHONY: all clean help run
