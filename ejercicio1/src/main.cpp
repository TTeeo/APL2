#include <params.hpp>
#include <funciones.hpp>

int main(int argc, char *argv[]) {

    try {
        Parametros parametros(argc, argv);
        crearJerarquia();
    } catch (const runtime_error& e) {
        cerr << "\033[31mError: " << e.what() << "\033[0m" << endl; 
        exit(EXIT_FAILURE);
    }
    

    return EXIT_SUCCESS;
}