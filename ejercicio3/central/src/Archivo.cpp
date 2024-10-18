#include <Archivo.hpp>

Archivo::Archivo(const string& nombre) {
    this->nombre=nombre;
}

string Archivo::getArchivo() const {
    return nombre;
}

