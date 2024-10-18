#include <Hilo.hpp>

int main(int argc, char *argv[])
{
    try {
        Parametros parametros(argc, argv);
        crearHilos(parametros);
    } catch (const exception& e) {
        cerr << "\033[31m" << "Error: " << e.what() << "\033[0m\n" << endl;;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}