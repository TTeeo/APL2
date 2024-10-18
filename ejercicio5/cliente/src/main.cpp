#include "cliente.hpp"
#include "params.hpp"

#define COLOR_ROJO "\033[31m"
#define RESET_COLOR "\033[0m"

int main(int argc, char *argv[]) {
  try {

    Parametros parametros(argc, argv);

    Cliente cliente(parametros.getNickname());

    cliente.crearSocket(parametros.getServidorIp(), parametros.getPuerto());
    cout << "PID: " << getpid() << endl;

    while (!cliente.juegoListoParaIniciar()) {
    }
    cliente.jugar();
  }

  catch (const exception &e) {
    cout << COLOR_ROJO << e.what() << RESET_COLOR << endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}