#include "params.hpp"

Parametros::Parametros(int argc, char *argv[]) {

  if (noSeIngresaronParametros(argc)) {
    throw invalid_argument("Error: No se ingresadon parametros.");
  }

  if (seSolicitoAyuda(argc, argv)) {
    mostrarAyuda();
    exit(EXIT_SUCCESS);
  } else {
    if (argc != ARGUMENTOS_ESPERADOS) {
      throw invalid_argument(
          "Error: Se esperaban " + to_string(ARGUMENTOS_ESPERADOS) +
          " argumentos pero se ingresaron " + to_string(argc) + " argumentos.");
    }

    string paramArchivo(argv[0]), paramCantidad(argv[2]), archivo(argv[1]),
        cantidad(argv[3]);

    if (validarParametroArchivo(paramArchivo, archivo) ==
        EstadoParametro::ParamArchivoNoCoincide) {

      swap(archivo, cantidad);
      swap(paramArchivo, paramCantidad);
    }

    procesarArgumentos(paramArchivo, archivo, paramCantidad, cantidad);
  }
}

EstadoParametro Parametros::validarParametroArchivo(const string &arg,
                                                    const string &dir) const {

  if (!esParametroArchivo(arg)) {
    return EstadoParametro::ParamArchivoNoCoincide;
  }

  filesystem::path ruta(dir);
  return filesystem::exists(ruta) && filesystem::is_regular_file(ruta)
             ? EstadoParametro::Exito
             : EstadoParametro::NoEsUnArchivoValido;
}

EstadoParametro Parametros::validarParametroCantidad(const string &arg,
                                                     const string &cant) const {

  if (!esParametroCantidad(arg)) {
    return EstadoParametro::ParamCantidadNoCoincide;
  }

  int numero;
  try {
    numero = stoi(cant);

  } catch (const invalid_argument &) {
    return EstadoParametro::NoEsUnNumero;
  } catch (const out_of_range &) {
    return EstadoParametro::NumeroFueraDeRango;
  }
  return numero > 0
             ? numero <=
                       Archivo::obtenerPreguntasDisponibles(getNombreArchivo())
                   ? EstadoParametro::Exito
                   : EstadoParametro::CantidadSuperaPreguntasDisponibles
             : EstadoParametro::NoEsUnNumeroPositivo;
}

void Parametros::procesarArgumentos(string &paramArchivo, string &archivo,
                                    string &paramCantidad, string &cantidad) {

  switch (validarParametroArchivo(paramArchivo, archivo)) {

  case EstadoParametro::ParamArchivoNoCoincide:
    throw invalid_argument("Error: El argumento " + paramArchivo +
                           " no coincide con -a|--archivo.");
    break;
  case EstadoParametro::NoEsUnArchivoValido:
    throw invalid_argument("Error: El argumento " + archivo +
                           " no es un archivo valido.");
    break;
  default:
    break;
  }

  setNombreArchivo(archivo);

  switch (validarParametroCantidad(paramCantidad, cantidad)) {

  case EstadoParametro::ParamCantidadNoCoincide:
    throw invalid_argument("Error: El argumento " + paramCantidad +
                           " no coincide con -c|--cantidad.");
    break;
  case EstadoParametro::NoEsUnNumero:
    throw invalid_argument("Error: El argumento " + cantidad +
                           " no es un numero.");
    break;
  case EstadoParametro::NumeroFueraDeRango:
    throw invalid_argument("Error: El argumento: " + cantidad +
                           " es un numero fuera de rango.");
    break;
  case EstadoParametro::NoEsUnNumeroPositivo:
    throw invalid_argument("Error: El argumento: " + cantidad +
                           " es un numero positivo.");
    break;

  case EstadoParametro::CantidadSuperaPreguntasDisponibles:
    throw invalid_argument(
        "Error: No disponemos de la cantidad de preguntas solicitadas.");

    break;
  default:
    break;
  }

  setcantidadPreguntas(stoi(cantidad));
}

bool Parametros::seSolicitoAyuda(int argc, char *argv[]) {

  if (argc != 1) {
    return false;
  }
  string pedido_ayuda(*argv);
  return !pedido_ayuda.compare("-h") || !pedido_ayuda.compare("--help") ? true
                                                                        : false;
}


void Parametros::mostrarAyuda() const {

  cout << "\033[34mAyuda del Servidor\033[0m\n"; // Título en azul
  cout << "Parámetros del servidor:\n";
  cout << "-a / --archivo    Archivo con las preguntas (Requerido).\n";
  cout << "-c / --cantidad   Cantidad de preguntas por partida (Requerido).\n";
  cout << "-h / --help       Muestra la ayuda del ejercicio.\n";
  cout << "Descripción: El servidor se encargará de gestionar las partidas de "
          "Preguntados, leyendo las preguntas desde el archivo especificado y "
          "enviándolas al cliente.\n\n";
}