#include "cliente.hpp"
#include "params.hpp"

#define COLOR_ROJO "\033[31m"
#define RESET_COLOR "\033[0m"

int main(int argc, char *argv[]) {
  try {

    Parametros parametros(argc, argv);

    Cliente cliente(parametros.getNickname());

    cout << "Para finalizar el cliente usa 'kill -SIGUSR1 " << getpid() << "'."
    << endl;
    cout << "Esperando sala disponible..." << endl;

    cliente.crearSocket(parametros.getServidorIp(), parametros.getPuerto(),
                        parametros.getNickname());

    cout << "\nSe ha ingresado a la sala, cuando haya suficientes jugadores se "
            "iniciara el juego."
         << endl;

    while (!cliente.juegoListoParaIniciar()) {
    }
    cliente.jugar();

    cout << "Aguardando que todos los jugadores terminen la partida..." << endl;

    cliente.mostrarResultados(cliente.obtenerResultados());
  }

  catch (const exception &e) {
    cout << COLOR_ROJO << e.what() << RESET_COLOR << endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
