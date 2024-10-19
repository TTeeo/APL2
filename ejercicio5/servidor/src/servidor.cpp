#include "servidor.hpp"

Servidor *Servidor::instanciaServidor = nullptr;

Servidor::Servidor(int cantJugadores, int cantPreguntas)
    : cantJugadoresMaximo(cantJugadores),
      cantPreguntasPorPartida(cantPreguntas) {

  semServidor = sem_open(NOMBRE_SEM_SERVIDOR, O_CREAT | O_EXCL, 0644, 0);

  if (semServidor == SEM_FAILED) {
    if (errno == EEXIST) {
      throw runtime_error("Error: El servidor ya está en ejecución.");
    } else {
      throw runtime_error("Error al crear el semáforo: ");
    }
  }
  instanciaServidor = this;
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  signal(SIGUSR1, Servidor::manejadorFinDeServidor);
}

void Servidor::crearSocket(int puerto, int cantUsuariosMaximo) {

  int opcion = 1;
  struct sockaddr_in direccion;
  socketServidor = socket(AF_INET, SOCK_STREAM, 0);
  if (socketServidor == 0) {
    throw runtime_error("Error: No se pudo crear el socket.");
  }

  if (setsockopt(socketServidor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                 &opcion, sizeof(opcion))) {
    close(socketServidor);
    throw runtime_error("Error: No se pudo configurar el socket.");
  }

  direccion.sin_family = AF_INET;         // IPv4
  direccion.sin_addr.s_addr = INADDR_ANY; // Acepta cualquier IP local
  direccion.sin_port = htons(puerto);     // Puerto a escuchar

  if (bind(socketServidor, (struct sockaddr *)&direccion, sizeof(direccion)) <
      0) {
    throw runtime_error("Error: No se pudo asociar el scoket a la direccion "
                        "IP y el puerto: " +
                        to_string(puerto));
  }
  if (listen(socketServidor, cantUsuariosMaximo) < 0) {
    close(socketServidor);
    throw runtime_error("Error: El listen fallo.");
  }
}
void Servidor::manejadorCliente(int sockCliente) {
  char buffer[TAM_BUFFER];

  ssize_t bytesRecibidos = recv(sockCliente, buffer, TAM_NICKNAME, 0);
  if (bytesRecibidos <= 0) {
    cout << "Cliente desconectado sin iniciar el juego" << endl;
    return;
  }
  MensajeCliente mensajeCliente;

  memcpy(&mensajeCliente, buffer,
         sizeof(((MensajeCliente *)nullptr)->nickname));

  puntajes[string(mensajeCliente.nickname)] = 0;
  socketClienteNickname[sockCliente] = string(mensajeCliente.nickname);

  MensajeServidor mensajeServidor;
  string mensajeAEnviar;

  mensajeServidor.partidaEnCurso = true;

  mensajeAEnviar = MensajesJuego::mensajeBienvenida.c_str();

  for (int turno = 0; turno < cantPreguntasPorPartida; turno++) {
    enviarPregunta(sockCliente, mensajeServidor, preguntasSeleccionadas[turno],
                   mensajeAEnviar);

    bytesRecibidos = recv(sockCliente, buffer, sizeof(int), 0);
    if (bytesRecibidos <= 0) {
      cout << "Cliente " << mensajeCliente.nickname << " se ha desconectado."
           << endl;
      return;
    }

    memcpy(&(mensajeCliente.opcionElegida), buffer, sizeof(int));
    if (bytesRecibidos == sizeof(int)) {
      mensajeAEnviar = procesarRespuestaCliente(
          mensajeCliente, preguntasSeleccionadas[turno].getOpcionCorrecta());
    }
  }

  mensajeServidor.partidaEnCurso = false;
  strcpy(mensajeServidor.mensaje, mensajeAEnviar.c_str());
  send(sockCliente, &mensajeServidor, sizeof(MensajeServidor), 0);

  cout << "Finalizo Partida con cliente " << mensajeCliente.nickname << endl;
}

string Servidor::procesarRespuestaCliente(MensajeCliente &msjCliente,
                                          int opcionCorrecta) {

  if (msjCliente.opcionElegida == opcionCorrecta) {

    puntajes[msjCliente.nickname]++;
    return MensajesJuego::mensajeRespuestaCorrecta;
  }
  return MensajesJuego::mensajeRespuestaIncorrecta;
}

void Servidor::enviarPregunta(int sockCliente, MensajeServidor &msjServidor,
                              const Pregunta pregunta,
                              string mensajeDefinidoPorServidor) const {
  strcpy(msjServidor.pregunta, pregunta.getPregunta().c_str());

  for (int i = 0; i < CANT_OPCIONES; i++) {
    strcpy(msjServidor.opciones[i], pregunta.getOpcionSegunPosicion(i).c_str());
  }
  strcpy(msjServidor.mensaje, mensajeDefinidoPorServidor.c_str());
  send(sockCliente, &msjServidor, sizeof(MensajeServidor), 0);
}

void Servidor::aceptarConexionNueva() {

  int socketCliente;
  do {
    struct sockaddr_in direccionCliente;
    socklen_t tamDireccionCliente =
        sizeof(direccionCliente); // No puedo pasar directamente el
                                  // sizeof(direccionCliente) en accept
    socketCliente = accept(socketServidor, (struct sockaddr *)&direccionCliente,
                           &tamDireccionCliente);
    if (socketCliente == -1) {
      if (errno == EBADF) {
        throw runtime_error("El servidor se cerro inesperadamente.");
      } else {
        throw runtime_error("Error al aceptar la conexión: " +
                            string(strerror(errno)));
      }
    }
    if (cantidadJugadoresConectados == cantJugadoresMaximo) {
      close(socketCliente);
      bool barrera = true;
      send(socketCliente, &barrera, sizeof(bool), 0);
    }
  } while (cantidadJugadoresConectados == cantJugadoresMaximo);

  bool barrera = false;
  send(socketCliente, &barrera, sizeof(bool), 0);

  string nicknameCliente = obtenerNicknameCliente(socketCliente);
  char mensaje[TAM_MSJ_SERVIDOR];

  if (!nicknameCliente.empty()) {

    if (!nicknameDuplicado(nicknameCliente)) {
      cantidadJugadoresConectados++;
      socketsClientes.emplace_back(socketCliente);
      puntajes[nicknameCliente] = 0;
      strcpy(mensaje,
             "0|Hemos recibido su nickname! Por favor aguarde hasta que "
             "el juego comience...");
      // cout << "Conexión aceptada." << endl;
    } else {
      strcpy(
          mensaje,
          "1|Lo sentimos! Ha ingresado un nickname que ya se ha registrado en "
          "el servidor, por favor intente de nuevo con uno diferente.");
    }

  } else {
    strcpy(mensaje, "2|Lo sentimos! No hemos podido recibir su nickname.");
  }
  send(socketCliente, mensaje, sizeof(mensaje), 0);
}

bool Servidor::nicknameDuplicado(const string &nicknameCliente) const {
  auto it = puntajes.find(nicknameCliente);
  return it != puntajes.end() ? true : false;
}

string Servidor::obtenerNicknameCliente(int socketCliente) const {

  char buffer[TAM_NICKNAME];

  int bytesRecibidos = recv(socketCliente, buffer, sizeof(buffer), 0);
  if (bytesRecibidos <= 0 || bytesRecibidos > TAM_MSJ_SERVIDOR) {
    return string("");
  }
  return string(buffer);
}
void Servidor::sacarClientesCaidos() {
  char buffer[1]; // Buffer vacío, no esperamos leer datos reales

  // Usamos un iterador para evitar problemas al eliminar elementos
  for (auto it = socketsClientes.begin(); it != socketsClientes.end();) {
    int resultado = recv(*it, buffer, sizeof(buffer), MSG_PEEK | MSG_DONTWAIT);

    if (resultado == 0) {
      // cout << "Cliente " << *it << " ha cerrado la conexión de manera
      // ordenada" << endl;
      close(*it);
      it = socketsClientes.erase(it); // Eliminar cliente y avanzar al siguiente
      cantidadJugadoresConectados--;
    } else if (resultado < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
      // Ocurrió un error en la conexión
      // cout << "Error en la conexión con el cliente " << *it << ": "
      //     << strerror(errno) << endl;
      close(*it);
      it = socketsClientes.erase(it); // Eliminar cliente y avanzar al siguiente
      cantidadJugadoresConectados--;
    } else {
      // No se ha detectado cierre de conexión, avanzar al siguiente cliente
      ++it;
    }
  }
}

void Servidor::jugar() {

  for (int sockCliente : socketsClientes) {

    hilosClientes.emplace_back(
        [this, sockCliente]() { manejadorCliente(sockCliente); });
  }
  cout << "Sala llena, iniciando juego..." << endl;
  for (thread &hilo : hilosClientes) {
    if (hilo.joinable()) {
      hilo.join();
    }
  }
}

void Servidor::enviarResultados() const {
  vector<pair<int, string>> vectorPuntajes;
  for (const auto &par : puntajes) {
    vectorPuntajes.emplace_back(
        par.second, par.first); // Invertir el orden: puntaje, nickname
  }

  sort(vectorPuntajes.begin(), vectorPuntajes.end(),
       [](const auto &a, const auto &b) {
         if (a.first != b.first)
           return a.first > b.first;
         return a.second < b.second;
       });

  vector<Resultado> resultados;
  Resultado resultado;

  int posicion = 1;

  for (size_t i = 0; i < vectorPuntajes.size(); i++) {

    strcpy(resultado.nickname, vectorPuntajes[i].second.c_str());
    resultado.puntaje = vectorPuntajes[i].first;

    if (i > 0 && vectorPuntajes[i].first == vectorPuntajes[i - 1].first) {
      resultado.posicion = resultados.back().posicion;
    } else {
      posicion = i + 1;
      resultado.posicion = posicion;
    }
    resultados.push_back(resultado);
  }
  char buffer[TAM_BUFFER];
  memcpy(buffer, &cantJugadoresMaximo, sizeof(int));

  for (size_t i = 0; i < resultados.size(); i++) {

    memcpy(buffer + sizeof(int) + i * sizeof(Resultado), &resultados[i],
           sizeof(Resultado));
  }

  for (const auto &par : socketClienteNickname) {
    if (send(par.first, buffer,
             sizeof(Resultado) * cantJugadoresMaximo + sizeof(int), 0) != -1) {
      cout << "Resultado enviado a: " << par.second << endl;
    }
  }
}

Servidor::~Servidor() { liberarRecursos(); }

void Servidor::liberarRecursos() const {
  if (semServidor != SEM_FAILED) {
    sem_close(semServidor);
    sem_unlink(NOMBRE_SEM_SERVIDOR);
  }

  for (const auto &socket : socketsClientes) {
    close(socket);
  }
  if (socketServidor != -1) {
    close(socketServidor);
  }
}

int Servidor::elegirPreguntaRandom(const vector<Pregunta> &preguntas) const {

  random_device rd;  // Semilla
  mt19937 gen(rd()); // Generador de Mersenne Twister

  uniform_int_distribution<> dis(0, preguntas.size() - 1);

  return dis(gen);
}

void Servidor::cargarPreguntas(vector<Pregunta> preguntas) {

  if (preguntas.size() == (unsigned)cantPreguntasPorPartida) {
    preguntasSeleccionadas = preguntas;
    return;
  }
  int posPregEscogida;
  for (int i = 0; i < cantPreguntasPorPartida; i++) {
    posPregEscogida = elegirPreguntaRandom(preguntas);
    preguntasSeleccionadas.push_back(preguntas[posPregEscogida]);
    preguntas.erase(preguntas.begin() + posPregEscogida);
  }
}

void Servidor::manejadorFinDeServidor(int signo) {

  if (signo == SIGUSR1) {

    instanciaServidor->liberarRecursos();

    if (instanciaServidor->cantidadJugadoresConectados ==
            instanciaServidor->cantJugadoresMaximo ||
        instanciaServidor->cantidadJugadoresConectados != 0) {
      cout << "\033[31mEl servidor se cerró inesperadamente.\033[0m" << endl;
      exit(EXIT_FAILURE);
    } else {
      cout << "\033[32mEl servidor se cerró exitosamente.\033[0m" << endl;
      exit(EXIT_SUCCESS);
    }
  }
}

void Servidor::confirmarPartida() {

  bool confimarCliente = true;
  for (const auto &sock : socketsClientes) {
    send(sock, &confimarCliente, sizeof(bool), 0);
  }
  rechazarClientes = thread(&Servidor::aceptarConexionNueva, this);
}

void Servidor::rechazarConexiones() {
  struct sockaddr_in direccionCliente;
  socklen_t tamDireccionCliente =
      sizeof(direccionCliente); // No puedo pasar directamente el
                                // sizeof(direccionCliente) en accept

  while (cantidadJugadoresConectados == cantJugadoresMaximo) {
    int socketCliente =
        accept(socketServidor, (struct sockaddr *)&direccionCliente,
               &tamDireccionCliente);
    if (socketCliente == -1) {
      if (errno == EBADF) {
        throw runtime_error("El servidor se cerro inesperadamente.");
      } else {
        throw runtime_error("Error al aceptar la conexión: " +
                            string(strerror(errno)));
      }
    }
    close(socketCliente);
  }
}

void Servidor::reiniciar() {

  cantidadJugadoresConectados = 0;
  for (auto &socket : socketsClientes) {
    close(socket);
  }
  socketsClientes.clear();
  puntajes.clear();
  socketClienteNickname.clear();
  preguntasSeleccionadas.clear();
  hilosClientes.clear();
  rechazarClientes.join();
}

/*
void Servidor::mostrarJugadoresConectados() const {
  static int jugadoresConectadosUltVez = 0;

  if (cantidadJugadoresConectados != jugadoresConectadosUltVez) {
    jugadoresConectadosUltVez++;
    cout << "Cantidad de jugadores en la sala: " << cantidadJugadoresConectados
         << endl;
  }
};
*/