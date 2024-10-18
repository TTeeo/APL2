# Definición de variables
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++20 -Iinclude  # Incluye la carpeta de headers
TARGET = bin/cliente.exe                         # Ruta del ejecutable
SOURCES = src/main.cpp src/cliente.cpp src/params.cpp # Fuentes
OBJECTS = $(patsubst src/%.cpp, obj/%.o, $(SOURCES))  # Archivos objeto

# Regla por defecto
all: $(TARGET)

# Regla para construir el ejecutable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Regla para compilar archivos .cpp a .o
obj/%.o: src/%.cpp $(CONFIG_HEADER)  # Dependencia del archivo de configuración
	mkdir -p obj    # Crea la carpeta de objetos si no existe
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpiar archivos objeto y el ejecutable
clean:
	rm -f obj/*.o $(TARGET)

# Regla para ejecutar el programa
run: $(TARGET)
	./$(TARGET) $(args)

.PHONY: all clean run