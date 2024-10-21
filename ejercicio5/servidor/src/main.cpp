#include "file.hpp"
#include "params.hpp"
#include "question.hpp"
#include "server.hpp"

using namespace std;

#define COLOR_ROJO "\033[31m"
#define RESET_COLOR "\033[0m"

int main(int argc, const char *argv[]) {
  try {

    // 1. Configuración de parámetros y carga de preguntas
    Parametros parametros(argc - 1, argv + 1);
    Archivo archivo(parametros.getNombreArchivoPreguntas());

    // Verificar si hay suficientes preguntas
    if (archivo.preguntasInsuficientes(parametros.getCantidadPreguntas())) {
      throw invalid_argument("No disponemos de las preguntas solicitadas");
    }

    // 2. Crear el servidor e inicializar el socket
    Servidor servidor(parametros.getPuerto(), parametros.getCantidadUsuarios(),
                      parametros.getCantidadPreguntas());
    servidor.crearSocket(parametros.getCantidadUsuarios());

    cout << "Servidor iniciado. Para finalizarlo usa 'kill -SIGUSR1 "
         << getpid() << "'." << endl;

    // 3. Se crea un hilo que aceptara o rechazara las conexiones de los
    // clientes.
    servidor.aceptarConexiones();
    int cantPartidasJugadas = 1;
    // 4. Se inicia un ciclo infinito hasta que se identifique la señal -SIGUSR1
    while (true) {

      // 5. Cargar preguntas al azar segun la cantidad ingresada por parametro
      // al servidor
      servidor.cargarPreguntas(archivo.getPreguntas());
      cout << "\nSala iniciada, esperando jugadores..." << endl;
      // 6. Esperar a que se llene la sala o que se cierre el servidor.
      while (!servidor.salaLlena()) {
      }

      // 7. Verificar que el servidor no se haya cerrado.

      cout << "\n"
           << string(10, '-') << " Mensajes del Servidor: Partida "
           << cantPartidasJugadas++ << " " << string(10, '-') << endl;

      // 8. Ejecutar el ciclo de juego
      servidor.jugar();

      // 9. Enviar los resultados de la partida
      servidor.enviarResultados();
      cout << "Partida finalizada." << endl;
      cout << string(50, '-') << endl;
      // 10. Reestablecer valores del servidor.
      servidor.reiniciar();
    }

  } catch (const ExcepcionAyuda &e) {
    cout << e.what() << endl;
  } catch (const std::exception &e) {
    cout << COLOR_ROJO << e.what() << RESET_COLOR << endl;
    return EXIT_FAILURE; // En caso de error, devolver fallo
  }

  return EXIT_SUCCESS;
}
