#include <vector>
#include <string>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>

#include "Archivo.hpp"

using namespace std;

class Directorio {
private:
    string ruta; // Almacena la ruta del directorio
    vector<Archivo> archivos; // Lista de archivos
    size_t pos=0;
public:
    Directorio();
    Directorio(const string& ruta);
    void cargarArchivos(); // Método para cargar archivos en el vector

    string getDirectorio() const;

    void setDirectorio(string& r);
    void reiniciarPos();

    void listarContenido() const; 
    bool obtenerArchivo(Archivo& archivo);
    size_t cantArchivos() const;
};