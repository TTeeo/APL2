#include <iostream>
#include <string>

using namespace std;

class Parametros {
private:
    bool seSolicitoAyuda(const string arg) const;
    void mostrarAyuda() const;
public:
    Parametros() = default;
    Parametros(int argc, char *argv[]);
};