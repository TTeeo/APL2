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
    cout << "Servidor iniciado. Para cerrarlo ingrese en otra consola 'kill -SIGUSR1 "<< getpid() << "'." << endl;

    // Bucle principal del servidor
    while (!servidor.cerrar()) {
      cout << "Esperando al cliente..." << endl;
      servidor.jugar();
    }

    return EXIT_SUCCESS;
  } catch (const exception &e) {
    cout << TEXTO_CONSOLA_COLOR_ROJO << e.what() << RESET_COLOR_CONSOLA << endl;
    return EXIT_FAILURE;
  }
}