#include "cliente.hpp"

Cliente *Cliente::instanciaCliente = nullptr;

Cliente::Cliente(string nombre) : nickname(nombre) {
  instanciaCliente = this;
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  signal(SIGUSR1, Cliente::manejadorFinCliente);
}

void Cliente::crearSocket(string ip, int puerto, string nickname) {

  descriptorSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (descriptorSocket < 0) {
    throw runtime_error("No se pudo crear el socket");
  }

  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(puerto);
  inet_pton(AF_INET, ip.c_str(), &server_address.sin_addr);

  if (connect(descriptorSocket, (struct sockaddr *)&server_address,
              sizeof(server_address)) < 0) {
    close(descriptorSocket);
    throw runtime_error("No se pudo conectar con el servidor.");
  }

  bool barrera = false;
  cout << "Hola " << endl;
  recv(descriptorSocket, &barrera, sizeof(bool), 0);
  cout << "Hola " << endl;
  if (barrera) {
    close(descriptorSocket);
    throw runtime_error("La partida ya esta empezada.");
  }

  char nicknameChr[nickname.length() + 1];
  strcpy(nicknameChr, nickname.c_str());

  if (send(descriptorSocket, nicknameChr, sizeof(nicknameChr), 0) == -1) {
    close(descriptorSocket);
    throw runtime_error("No se pudo enviar el nickname al servidor");
  }
  char mensajeServidor[TAM_MSJ_SERVIDOR];

  int bytesRecibidos =
      recv(descriptorSocket, mensajeServidor, sizeof(mensajeServidor), 0);

  if (bytesRecibidos == -1) {
    close(descriptorSocket);
    throw runtime_error("Error al recibir datos: " + string(strerror(errno)));
  }
  if (bytesRecibidos == 0) {
    close(descriptorSocket);
    throw runtime_error("El servidor ha cerrado la conexion.");
  }
  string msjServidor(mensajeServidor + 2);
  int codigoEstado = mensajeServidor[0] - '0';

  if (codigoEstado != 0) {
    throw runtime_error(msjServidor);
  }
}

int Cliente::obtenerRespuestaCliente() const {

  string respuesta;
  int numeroOpcion = -1;

  do {
    cout << "Ingrese la opcion correcta[1-3]: ";
    cin >> respuesta;

    try {
      numeroOpcion = stoi(respuesta);
    } catch (const exception &) {
      cout << "La opcion ingresada no es un numero. Por favor, vuelva a "
              "intentar!"
           << endl;
    }
    if (!esOpcionValida(numeroOpcion)) {
      cout << "La opcion ingresada no es valida, por favor ingrese una opcion "
              "correcta."
           << endl;
    }
  } while (!esOpcionValida(numeroOpcion));

  return numeroOpcion;
}

bool Cliente::esOpcionValida(int opcion) const {
  return opcion > 0 && opcion <= CANT_OPCIONES;
}

void Cliente::mostrarResultadoJugador(Resultado &res) const {

  cout << setw(10) << res.posicion << setw(TAM_NICKNAME) << res.nickname
       << setw(10) << res.puntaje << endl;
}

void Cliente::MostrarPregunta(MensajeServidor &msjServidor) const {

  cout << "\n" << msjServidor.mensaje << endl;
  cout << msjServidor.pregunta << "\n" << endl;

  for (const auto &opt : msjServidor.opciones) {
    cout << opt << endl;
  }
}

void Cliente::jugar() {

  char buffer[TAM_BUFFER];
  strcpy(buffer, nickname.c_str());

  if (send(descriptorSocket, buffer, TAM_NICKNAME, 0) == -1) {
    throw runtime_error("Error: No se pudo enviar el nickname." + nickname);
  }
  MensajeServidor msjServidor;
  int respuesta, bytesRecibidos;

  while (true) {
    bytesRecibidos = recv(descriptorSocket, buffer, sizeof(MensajeServidor), 0);
    if (bytesRecibidos == 0) {
      throw runtime_error("Error: El servidor se ha desconectado.");
    }
    if (bytesRecibidos < 0) {
      throw runtime_error("Error al recibir datos: " + string(strerror(errno)));
    }
    memcpy(&msjServidor, buffer, sizeof(MensajeServidor));

    if (!msjServidor.partidaEnCurso) {
      cout << msjServidor.mensaje << "\n\n" << endl;
      break;
    }

    MostrarPregunta(msjServidor);
    respuesta = obtenerRespuestaCliente();

    if (send(descriptorSocket, &respuesta, sizeof(int), 0) == -1) {
      throw runtime_error("Error: El servidor se ha desconectado o no se ha "
                          "podido enviar la respuesta.");
    }
  }

  if (msjServidor.partidaEnCurso == false) {
    if (recv(descriptorSocket, buffer, TAM_BUFFER, 0) <= 0) {
      throw runtime_error("Error: No se han podido recibir los resultados");
    }
    int cantJugadores;
    memcpy(&cantJugadores, buffer, sizeof(int));

    cout << left; // Alinear a la izquierda
    cout << setw(10) << "Posición" << setw(TAM_NICKNAME) << "Jugador"
         << setw(10) << "Puntaje" << endl;
    cout << string(81, '-') << endl; // Línea separadora
    Resultado res;
    bool clienteGano = false;

    for (int i = 0; i < cantJugadores; i++) {
      memcpy(&res, buffer + sizeof(int) + sizeof(Resultado) * i,
             sizeof(Resultado));

      mostrarResultadoJugador(res);
      if (string(res.nickname) == this->nickname && res.posicion == 1) {
        clienteGano = true;
      }
    }
    cout << string(81, '-') << endl;
    cout << (clienteGano ? "\nFelicidades, has ganado!!"
                         : "\nTienes que mejorar más!! Has perdido.")
         << "\n\nLa partida ha finalizado!! Gracias por jugar con nosotros.\n"
         << endl;
  } else {
    cout << "\n\nNo se pudo completar la partida." << endl;
  }
}

bool Cliente::juegoListoParaIniciar() const {

  bool clientesListos = false;
  int bytesRecibidos = recv(descriptorSocket, &clientesListos, sizeof(bool), 0);

  if (bytesRecibidos < 0) {

    switch (errno) {
    case EAGAIN | EWOULDBLOCK:
      // No hay datos disponibles en este momento, pero la conexión sigue activa
      return false;
    case EBADF:
      throw runtime_error("Descriptor de socket no válido. El servidor puede "
                          "haberse desconectado.");
    case ECONNRESET:
      throw runtime_error("La conexión fue restablecida por el servidor.");
    case EINTR:
      // La llamada fue interrumpida por una señal, se puede reintentar
      return false;
    default:
      throw runtime_error(
          "Error desconocido al intentar recibir datos del servidor.");
    }
  } else if (bytesRecibidos == 0) {
    throw runtime_error("El servidor se ha desconectado de manera ordenada.");
  }

  return clientesListos;
}

Cliente::~Cliente() {

  if (descriptorSocket != -1) {
    if (errno != ECONNREFUSED) {
      cout << "Conexión cerrada." << endl;
    }
    close(descriptorSocket);
  }
}
void Cliente::cerrarSocket() {

  if (descriptorSocket != -1) {
    close(descriptorSocket);
  }
}

void Cliente::manejadorFinCliente(int signo) {
  if (signo == SIGUSR1) {
    if (instanciaCliente->descriptorSocket != -1) {
      close(instanciaCliente->descriptorSocket);
    }

    cout << "\033[31mEl cliente se cerro de manera inesperada.\033[0m"
         << std::endl;

    exit(EXIT_FAILURE);
  }
}