#include <Directorio.hpp>
#include <Cadena.hpp>

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

class Parametros {
private:
    Directorio directorio;
    int nroThreads;
    Cadena cadena;

    bool directorioProcesado = false;
    bool nroThreadsProcesado = false;
    bool cadenaProcesado = false;

    void setDirectorio(Directorio directorio);
    void setNroThreads(int nroThreads);
    void setCadena(Cadena cadena);

    void noSeIngresaronParametros(int argc);

    void parametrosObligatorios();

    bool esParametroDirectorio(const string arg) const;
    bool esParametrNroThreads(const string arg) const;
    bool esParametrCadena(const string arg) const;

    void validarDirectorio(const string &direc) const;
    void validarNroThread(const string& str) const;

    bool seSolicitoAyuda(const string arg) const;
    void mostrarAyuda() const;

public:
    Parametros() = default;
    Parametros(int argc, char *argv[]);

    Directorio getDirectorio() const;
    int getNroThreads() const;
    Cadena getCadena() const;

    void mostrarParametros() const;
};