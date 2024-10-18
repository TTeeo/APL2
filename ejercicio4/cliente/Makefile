# Definición de variables
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++20 -Iinclude
TARGET = bin/ejercicio4.exe
SOURCES = src/main.cpp src/params.cpp src/cliente.cpp
OBJECTS = $(patsubst src/%.cpp, obj/%.o, $(SOURCES))

# Regla por defecto
all: $(TARGET)

# Regla para construir el ejecutable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Regla para compilar archivos .cpp a .o
obj/%.o: src/%.cpp $(CONFIG_HEADER)
	mkdir -p obj
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpiar archivos objeto y el ejecutable
clean:
	rm -f obj/*.o $(TARGET)

# Regla para mostrar la ayuda
help:
	@echo "Uso: $(TARGET) -h|--help"

# Regla para ejecutar el programa
run: $(TARGET)
	./$(TARGET) $(args)

.PHONY: all clean run