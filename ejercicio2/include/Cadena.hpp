#include <string>
#include <iostream>

using namespace std;

class Cadena {
private:
    string texto; 
public:
    Cadena(const string& texto);
    Cadena();
    void setCadena(string& texto);
    string getCadena() const;
    void mostrar() const;
    bool estaContenidaEn(const Cadena& otra) const;
};