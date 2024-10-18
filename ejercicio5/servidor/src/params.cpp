#include "params.hpp"
Parametros::Parametros(int argc, const char *argv[]) {

  if (argc == 0) {
    throw invalid_argument("Error: No se ingresaron parametros.");
  }
  if (seSolicitoAyuda(argc, *argv)) {
    throw ExcepcionAyuda();
  }
  if (argc != ARGUMENTOS_ESPERADOS) {
    throw invalid_argument("Error: Parametros insuficientes");
  }
  inicializarOpciones();
  for (int i = 0; i < argc; i += 2) {
    ejecutarOpcion(string(argv[i]), argv[i + 1]);
  }
}
void Parametros::inicializarOpciones() {

  opciones["-p"] = [this](const char *arg) { opcionPuerto(arg); };
  opciones["--puerto"] = [this](const char *arg) { opcionPuerto(arg); };
  opciones["-u"] = [this](const char *arg) { opcionUsuarios(arg); };
  opciones["--usuarios"] = [this](const char *arg) { opcionUsuarios(arg); };
  opciones["-a"] = [this](const char *arg) { opcionArchivo(arg); };
  opciones["--archivo"] = [this](const char *arg) { opcionArchivo(arg); };
  opciones["-c"] = [this](const char *arg) { opcionCantidad(arg); };
  opciones["--cantidad"] = [this](const char *arg) { opcionCantidad(arg); };
}

void Parametros::opcionPuerto(string opcion) {

  try {
    puerto = stoi(opcion);
    if (puerto < 0 || puerto > 65535) {
      throw("Error: El puerto debe estar entre 0 y 65535");
    }
  } catch (const exception &) {
    throw invalid_argument("Error: El puerto no es un numero");
  }
}

void Parametros::opcionUsuarios(string opcion) {
  try {
    cantUsuarios = stoi(opcion);

  } catch (const exception &) {
    throw invalid_argument("Error: Los usuarios ingresados no son validos");
  }

  if (cantUsuarios <= 0 || cantUsuarios > USUARIOS_SOPORTADOS) {
    throw invalid_argument("Error: No soportamos esa cantidad de usuarios");
  }
}
void Parametros::opcionCantidad(string opcion) {
  try {
    cantPreguntasPorPartida = stoi(opcion);
  } catch (const exception &) {
    throw invalid_argument(
        "Error: La cantidad de preguntas ingresadas no son validas");
  }

  if (cantPreguntasPorPartida <= 0) {
    throw invalid_argument("Error: No soportamos esa cantidad de preguntas");
  }
}
void Parametros::opcionArchivo(string opcion) {
  if (!filesystem::exists(opcion)) {
    throw invalid_argument("Error: El archivo ingresado no existe.");
  }
  archivoPreguntas = opcion;
}

void Parametros::ejecutarOpcion(string clave, const char *arg) {
  auto it = opciones.find(clave);
  if (it != opciones.end()) {
    it->second(arg); // Llama a la función almacenada
  } else {
    throw invalid_argument("Error: La opcion ingresada " + clave +
                           " no es valida");
  }
}