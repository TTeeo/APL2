#include <funciones.hpp>

int main(int argc, char *argv[]) {
    try {
        Parametros parametros(argc, argv);
        enviarHuellas(parametros);
    } catch (const std::runtime_error &e) {
        cerr << "\033[31mError: " << e.what() << "\033[0m" << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}