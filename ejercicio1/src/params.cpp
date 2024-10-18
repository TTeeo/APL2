#include <params.hpp>

Parametros::Parametros(int argc, char *argv[]) 
{
    if(argc==2 && seSolicitoAyuda(argv[1])) {
        mostrarAyuda();    
        exit (EXIT_SUCCESS);  
    } else if(argc>=2) {
        throw runtime_error("Parametros invalidos.");
    }   
}

bool Parametros::seSolicitoAyuda(const string arg) const 
{
    return !arg.compare("--help") || !arg.compare("-h") ? true : false;
}

void Parametros::mostrarAyuda() const 
{
    cout << "\033[34mAyuda\033[0m\n"; // Título en azul
    cout << "Parámetros:\n";
    cout << "-h / --help           Muestra la ayuda del ejercicio.\n";
    cout << "Descripción: Se genera una jerarquia de procesos.\n\n";
}

