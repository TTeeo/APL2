#include "Directorio.hpp"

Directorio::Directorio(const string& ruta) : ruta(ruta) {}

Directorio::Directorio() : ruta("") {}

void Directorio::cargarArchivos() {
    DIR* dir = opendir(ruta.c_str());
    if (dir == nullptr) {
        throw runtime_error("No se pudo abrir directorio.");
    }

    struct dirent* entry;
    struct stat fileStat;

    while ((entry = readdir(dir)) != nullptr) {
        // Ignorar los directorios '.' y '..'
        if (entry->d_name[0] == '.') {
            continue;
        }
        // Obtener la ruta completa del archivo
        string filePath = ruta + "/" + entry->d_name;

        // Obtener información del archivo
        if (stat(filePath.c_str(), &fileStat) == 0) {
            // Verificar si es un archivo regular
            if (S_ISREG(fileStat.st_mode)) {
                // Crear un objeto Archivo y agregarlo al vector
                archivos.emplace_back(entry->d_name);
            }
        } else {
            throw runtime_error("No se pudo obtener información de los archivos");
        }
    }
    closedir(dir);
}

void Directorio::listarContenido() const {
    cout << "Contenido de " << ruta << ":\n";
    
    for (const auto& archivo : archivos) {
        cout << "- " << archivo.getNombre() << "\n";
    }
}

bool Directorio::obtenerArchivo(Archivo& archivo) {
    if (pos < archivos.size()) {
        archivo = archivos[pos]; 
        pos++;
        return true;
    }
    return false;
}

size_t Directorio::cantArchivos() const {
    return archivos.size();
}


string Directorio::getDirectorio() const {
    return ruta;
}


void Directorio::setDirectorio(string& r) {
    ruta = r;
}

void Directorio::reiniciarPos() {
    pos=0;
}
