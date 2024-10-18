# Usar la imagen base de Ubuntu 24.10
FROM ubuntu:24.10

# Establecer el directorio de trabajo dentro del contenedor
WORKDIR /apl

# Instalar las dependencias necesarias para compilar C++ (gcc y make)
RUN apt-get update && apt-get install -y \
    g++ \
    make \
    && apt-get clean

# Copiar los archivos de los ejercicios al directorio /apl dentro del contenedor
COPY ./ejercicio1 /apl/ejercicio1
COPY ./ejercicio2 /apl/ejercicio2
COPY ./ejercicio3 /apl/ejercicio3
COPY ./ejercicio4 /apl/ejercicio4
COPY ./ejercicio5 /apl/ejercicio5

# Indicar el comando por defecto al iniciar el contenedor (opcional)
CMD ["/bin/bash"]