#include <string>
#include <iostream>

using namespace std;

class Archivo {
private:
    string nombre;
public:
    Archivo() {this->nombre="";};
    Archivo(const string& nombre);
    string getArchivo() const;
};