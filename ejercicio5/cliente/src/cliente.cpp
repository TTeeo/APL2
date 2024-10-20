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
  TransmisionMensajes::recibirMensaje(descriptorSocket, &barrera, sizeof(bool),
                                      0);

  if (barrera) {
    throw runtime_error("La partida ya esta empezada.");
  }

  char nicknameChr[nickname.length() + 1];
  strcpy(nicknameChr, nickname.c_str());

  TransmisionMensajes::enviarMensaje(descriptorSocket, nicknameChr,
                                     sizeof(nicknameChr));

  ComunicacionNickname comunicacion;
  TransmisionMensajes::recibirMensaje(descriptorSocket, &comunicacion,
                                      sizeof(comunicacion), 0);

  if (comunicacion.codigoEstado != COMUNICACION_NICKNAME_EXITO) {
    throw runtime_error(string(comunicacion.mensaje));
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

      if (!esOpcionValida(numeroOpcion)) {
        cout
            << "La opcion ingresada no es valida, por favor ingrese una opcion "
               "correcta."
            << endl;
      }
    } catch (const exception &) {
      cout << "La opcion ingresada no es un numero. Por favor, vuelva a "
              "intentar!"
           << endl;
      continue;
    }

  } while (!esOpcionValida(numeroOpcion));

  return numeroOpcion;
}

bool Cliente::esOpcionValida(int opcion) const {
  return opcion > 0 && opcion <= CANT_OPCIONES;
}

void Cliente::jugar() {

  MensajeServidor msjServidor;
  int respuesta;

  while (true) {
    TransmisionMensajes::recibirMensaje(descriptorSocket, &msjServidor,
                                        sizeof(MensajeServidor), 0);

    if (!msjServidor.partidaEnCurso) {
      cout << msjServidor.mensaje << "\n\n" << endl;
      break;
    }
    cout << "\n" << msjServidor.mensaje << endl;
    mostrarPregunta(msjServidor);
    respuesta = obtenerRespuestaCliente();
    TransmisionMensajes::enviarMensaje(descriptorSocket, &respuesta,
                                       sizeof(int));
  }
}

vector<Resultado> Cliente::obtenerResultados() {
  int cantJugadoresTotales;

  TransmisionMensajes::recibirMensaje(descriptorSocket, &cantJugadoresTotales,
                                      sizeof(int), 0);

  char buffer[TAM_BUFFER];
  int resultadosALeer;
  bool quedanMensajes;
  Resultado res;
  const char *inicioLecturaDatos = buffer + sizeof(int) + sizeof(bool);
  vector<Resultado> retorno;

  do {
    TransmisionMensajes::recibirMensaje(descriptorSocket, buffer,
                                        sizeof(buffer), 0);

    memcpy(&resultadosALeer, buffer, sizeof(int));
    memcpy(&quedanMensajes, buffer + sizeof(int), sizeof(bool));
    for (int i = 0; i < resultadosALeer; i++) {
      memcpy(&res, inicioLecturaDatos + sizeof(Resultado) * i,
             sizeof(Resultado));
      retorno.push_back(res);
    }
  } while (quedanMensajes);

  return retorno;
}

void Cliente::mostrarResultados(vector<Resultado> resultados) const {
  cout << left; // Alinear a la izquierda
  cout << setw(10) << "Posición" << setw(TAM_NICKNAME) << "Jugador" << setw(10)
       << "Puntaje" << endl;
  cout << string(81, '-') << endl; // Línea separadora

  bool clienteGano = false;

  for (const auto &res : resultados) {
    cout << setw(10) << res.posicion << setw(TAM_NICKNAME) << res.nickname
         << setw(10) << res.puntaje << endl;
    if (string(res.nickname) == this->nickname && res.posicion == 1) {
      clienteGano = true;
    }
  }

  cout << string(81, '-') << endl;
  cout << (clienteGano ? "\nFelicidades, has ganado!!"
                       : "\nTienes que mejorar más!! Has perdido.")
       << "\n\nLa partida ha finalizado!! Gracias por jugar con nosotros.\n"
       << endl;
}

void Cliente::mostrarPregunta(MensajeServidor &msjServidor) const {

  cout << msjServidor.pregunta << "\n" << endl;

  for (const auto &opt : msjServidor.opciones) {
    cout << opt << endl;
  }
}
bool Cliente::juegoListoParaIniciar() const {

  bool clientesListos = false;

  // No hay datos disponibles en este momento, pero la conexión sigue activa
  if (TransmisionMensajes::recibirMensaje(descriptorSocket, &clientesListos,
                                          sizeof(bool),
                                          0) == DATOS_NO_DISPONIBLES) {
    return false;
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