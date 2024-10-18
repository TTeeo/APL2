#include <Archivo.hpp>

Archivo::Archivo(const string& nombre) : nombre(nombre) {}

string Archivo::getNombre() const {
    return nombre;
}
