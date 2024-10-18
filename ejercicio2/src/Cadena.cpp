#include <Cadena.hpp>

Cadena::Cadena(const string& t) : texto(t) {}

Cadena::Cadena() : texto("") {}

void Cadena::mostrar() const {
    cout << texto << endl;
}

void Cadena::setCadena(string& t) {
    texto = t;
}

string Cadena::getCadena() const {
    return texto;
}

bool Cadena::estaContenidaEn(const Cadena& otra) const {
    return (otra.texto).find(texto) != string::npos;
}