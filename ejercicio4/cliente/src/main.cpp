#include "client.hpp"
#include "params.hpp"
#include <iostream>

using namespace std;

int main(int argc, const char *argv[]) {
  try {

    Parametros params(argc - 1, argv + 1);

    Cliente cliente;
    cliente.iniciar();
    cout << "Cliente iniciado. Si lo desea puede cerrarlo usando: 'kill "
            "-SIGUSR1 "
         << getpid() << "'." << endl;
    int puntaje = cliente.jugar();

    cout << "Muchas gracias por jugar con nosotros "
         << params.getNombreUsuario() << "! Su puntaje fue de: " << puntaje
         << ". Hasta la proxima!" << endl;

    return EXIT_SUCCESS;

  } catch (const exception &e) {
    cout << TEXTO_CONSOLA_COLOR_ROJO << e.what() << RESET_COLOR_CONSOLA << endl;
    return EXIT_FAILURE;
  }
}
