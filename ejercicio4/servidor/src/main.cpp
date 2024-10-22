#include "file.hpp"
#include "params.hpp"
#include "question.hpp"
#include "server.hpp"

int main(int argc, char **argv) {
  try {
    // Iniciar y procesar parámetros
    Parametros parametros(argc - 1, argv + 1);

    // Iniciar servidor con las preguntas
    Servidor servidor(Archivo::obtenerPreguntas(parametros.getNombreArchivo()),
                      parametros.getCantidadPreguntas());
    servidor.iniciar();

    // Mostrar el PID del proceso
    cout << "Servidor iniciado. Para cerrarlo ingrese en otra consola 'kill "
            "-SIGUSR1 "
         << getpid() << "'." << endl;

    // Bucle principal del servidor
    while (true) {
      cout << "Esperando al cliente..." << endl;
      pause();
      if (servidor.cerrar()) {
        break;
      }

      cout << "Cliente conectado. Se iniciara la partida" << endl;
      try {
        servidor.jugar();
      } catch (const runtime_error &e) {
        cout << "\033[31m" << e.what() << "\033[0m" << endl;
      }
    }

    cout << "Cerrando servidor..." << endl;
    servidor.liberarRecursos();

    return EXIT_SUCCESS;
  } catch (const exception &e) {
    cout << TEXTO_CONSOLA_COLOR_ROJO << e.what() << RESET_COLOR_CONSOLA << endl;
    return EXIT_FAILURE;
  }
}