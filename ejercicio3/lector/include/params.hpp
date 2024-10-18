#include <Archivo.hpp>
#include <filesystem>
#include <fstream>

class Parametros {
private:
    int numero;
    int segundos;
    int mensajes;
    Archivo ids;

    bool numeroProcesado = false;
    bool segundosProcesado = false;
    bool mensajesProcesado = false;
    bool idsProcesado = false;

    void setNumero(const int numero);
    void setSegundos(const int segundos);
    void setMensajes(const int mensajes);
    void setIds(const Archivo ids);

    void noSeIngresaronParametros(int argc);

    void parametrosObligatorios();

    bool esParametroNumero(const string arg) const;
    bool esParametroSegundos(const string arg) const;
    bool esParametroMensajes(const string arg) const;
    bool esParametroIds(const string arg) const;

    void validarEnteros(const string &direc);
    void validarIds(const string &direc);

    bool seSolicitoAyuda(const string arg) const;
    void mostrarAyuda() const;

public:
    Parametros() = default;
    Parametros(int argc, char *argv[]);

    int getNumero() const;
    int getSegundos() const;
    int getMensajes() const;
    Archivo getIds() const;

    void mostrarParametros() const;
};