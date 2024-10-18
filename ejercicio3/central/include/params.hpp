#include <Archivo.hpp>
#include <filesystem>
#include <fstream>

class Parametros {
private:
    Archivo log;
    Archivo ids;

    bool logProcesado = false;
    bool idsProcesado = false;

    void setLog(const Archivo log);
    void setIds(const Archivo ids);

    void noSeIngresaronParametros(int argc);

    void parametrosObligatorios();

    bool esParametroLog(const string arg) const;
    bool esParametroIds(const string arg) const;

    void validarLog(const string &direc);
    void validarIds(const string &direc);

    bool seSolicitoAyuda(const string arg) const;
    void mostrarAyuda() const;

public:
    Parametros() = default;
    Parametros(int argc, char *argv[]);

    Archivo getLog() const;
    Archivo getIds() const;

    void mostrarParametros() const;
};