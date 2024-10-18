#include <string>
#include <iostream>

using namespace std;

class Archivo {
private:
    string nombre;
public:
    Archivo(const string& nombre);
    string getNombre() const;
};
