#include "params.hpp"

Parametros::Parametros(int argc, char *argv[]) {
  static struct option opcionesLargas[] = {
      {"nickname", required_argument, 0, 'n'},
      {"puerto", required_argument, 0, 'p'},
      {"servidor", required_argument, 0, 's'},
      {"help", no_argument, 0, 'h'},
      {0, 0, 0, 0} // Terminador
  };
  nickname = "";
  servidorIp = "";
  puerto = -1;

  int indiceOpcion = 0, opt;
  if (argc != CANT_ARGS_AYUDA && argc != CANT_ARGS_ESPERADOS) {
    throw invalid_argument(
        "Error: La cantidad de parametros no es la correcta.");
    exit(1);
  }

  while ((opt = getopt_long(argc, argv, "n:p:s:h", opcionesLargas,
                            &indiceOpcion)) != -1) {
    switch (opt) {
    case 'n':
      nickname = optarg;
      break;
    case 'p':
      try {
        puerto = stoi(optarg);
      } catch (const exception &e) {
        throw invalid_argument("El puerto ingresado no es un numero.");
      }
      break;
    case 's':
      servidorIp = optarg;
      break;
    case 'h':

      if (argc == 2) {
        mostrarAyuda();
        exit(EXIT_SUCCESS);
      }
      break;
    case '?':
      throw invalid_argument(
          "Error: Opción no reconocida o falta de argumento.");
    }
  }

  if (nickname.empty()) {
    throw invalid_argument(
        " Error: El parámetro -n / --nickname es requerido.");
  }

  if (servidorIp.empty()) {
    throw invalid_argument("Error: El parámetro -s / --servidor es requerido.");
  }

  if (puerto == -1) {
    throw invalid_argument("Error: El parámetro -p / --puerto es requerido.");
  }
}

void Parametros::mostrarAyuda() const {
  cout << "Uso: cliente [opciones]\n"
       << "Opciones:\n"
       << "  -n, --nickname    Nickname del usuario (Requerido)\n"
       << "  -p, --puerto      Número de puerto del servidor (Requerido)\n"
       << "  -s, --servidor    Dirección IP o nombre del servidor "
          "(Requerido)\n"
       << "  -h, --help        Muestra la ayuda del ejercicio\n"
       << "Ejemplo de uso del cliente:\n"
       << "  ./bin/cliente.exe -n Teo -p 8080 -s 127.0.0.1\n"
       << "  ./bin/cliente.exe --nickname Teo --puerto 8080 --servidor "
          "127.0.0.1\n"
       << "Consideraciones:\n"
       << "   -Para cerrar el cliente puede usar la senal SIGUSR1.\n"
       << endl;
}