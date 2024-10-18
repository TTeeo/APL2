#include "params.hpp"

Parametros::Parametros(int argc, const char *argv[]) {

  if (argc == 0) {
    throw invalid_argument("Error: No se ingresadon parametros.");
  }

  string pedidoAyuda(*argv);
  if (!pedidoAyuda.compare("-h") || !pedidoAyuda.compare("--help")) {
    if (argc == 1) {
      mostrarAyuda();
      exit(EXIT_SUCCESS);
    } else {
      throw invalid_argument(
          "Si usaste -h|--help revisa que no haya mas parametros ingresados.");
    }
  }

  if (argc != ARGUMENTOS_ESPERADOS) {
    throw invalid_argument(
        "Error: Se esperaban " + to_string(ARGUMENTOS_ESPERADOS) +
        " argumentos pero se ingresaron " + to_string(argc) + " argumentos.");
  }

  string paramNickname(argv[0]), nickname(argv[1]);

  if (paramNickname.compare("-n") && paramNickname.compare("--nickname")) {
    throw invalid_argument("Error: El argumento " + paramNickname +
                           " no coincide con -n|--nickname.");
  }

  this->nombreUsuario = nickname;
}

void Parametros::mostrarAyuda() const {

  cout << "\033[34mAyuda del Cliente\033[0m\n"; // Título en azul
  cout << "Parámetros del cliente:\n";
  cout << "-n / --nickname   Nickname del usuario (Requerido).\n";
  cout << "-h / --help       Muestra la ayuda del ejercicio.\n";
  cout << "Descripción: El cliente se encargará de mostrar las preguntas "
          "al jugador y registrar sus respuestas, comunicándose con el "
          "servidor.\n\n";
}