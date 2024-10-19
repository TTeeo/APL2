#include "file.hpp"
#include "params.hpp"
#include "question.hpp"
#include "servidor.hpp"

using namespace std;

#define COLOR_ROJO "\033[31m"
#define RESET_COLOR "\033[0m"

int main(int argc, const char *argv[]) {
  try {
    // 1. Mostrar el PID del proceso

    // 2. Configuración de parámetros y carga de preguntas
    Parametros parametros(argc - 1, argv + 1);
    Archivo archivo(parametros.getNombreArchivoPreguntas());

    // Verificar si hay suficientes preguntas
    if (archivo.preguntasInsuficientes(parametros.getCantidadPreguntas())) {
      throw invalid_argument("No disponemos de las preguntas solicitadas");
    }

    // 3. Crear el servidor e inicializar el socket
    Servidor servidor(parametros.getCantidadUsuarios(),
                      parametros.getCantidadPreguntas());
    servidor.crearSocket(parametros.getPuerto(),
                         parametros.getCantidadUsuarios());
    cout << "Servidor iniciado. Para finalizarlo usa 'kill -SIGUSR1 "
         << getpid() << "'." << endl;

    while (true) {
      cout << "\nSala iniciada, esperando jugadores..." << endl;
      // 4. Cargar preguntas al servidor
      servidor.cargarPreguntas(archivo.getPreguntas());

      // 5. Ciclo principal: aceptar conexiones hasta que la sala esté llena o
      // se cierre el servidor

      while (!servidor.salaLlena()) {
        servidor.aceptarConexionNueva();
        servidor.sacarClientesCaidos();
        // servidor.mostrarJugadoresConectados();
      }

      // 6. Confirmar que la partida va a comenzar
      servidor.confirmarPartida();

      // 7. Ejecutar el ciclo de juego
      servidor.jugar();

      // 8. Manejar clientes caídos durante la partida
      servidor.sacarClientesCaidos();

      // 9. Enviar los resultados de la partida
      servidor.enviarResultados();

      cout << "Partida finalizada." << endl;

      servidor.reiniciar();
    }
  } catch (const ExcepcionAyuda &e) {
    cout << e.what() << endl;
  } catch (const std::exception &e) {
    cout << COLOR_ROJO << e.what() << RESET_COLOR << endl;
    return EXIT_FAILURE; // En caso de error, devolver fallo
  }

  return EXIT_SUCCESS; // Si todo salió bien, devolver éxito
}
