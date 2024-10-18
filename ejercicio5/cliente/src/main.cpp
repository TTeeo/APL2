#include "cliente.hpp"
#include "params.hpp"

#define COLOR_ROJO "\033[31m"
#define RESET_COLOR "\033[0m"

int main(int argc, char *argv[]) {
  try {

    Parametros parametros(argc, argv);

    Cliente cliente(parametros.getNickname());

    cliente.crearSocket(parametros.getServidorIp(), parametros.getPuerto());
    cout << "Para finalizar el cliente usa 'kill -SIGUSR1 26177 " << getpid() << "'." << endl;
    cout << "\nSe ha ingresado a la sala, cuando haya suficientes jugadores se iniciara el juego." << endl;

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