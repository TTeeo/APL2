#include "server.hpp"

Servidor *Servidor::instanciaServidor = nullptr;

Servidor::Servidor(int puerto, int cantJugadores, int cantPreguntas)
    : puertoUtilizado(puerto), cantJugadoresMaximo(cantJugadores),
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

void Servidor::crearSocket(int cantUsuariosMaximo) {
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

  direccion.sin_family = AF_INET;              // IPv4
  direccion.sin_addr.s_addr = INADDR_ANY;      // Acepta cualquier IP local
  direccion.sin_port = htons(puertoUtilizado); // Puerto a escuchar

  if (bind(socketServidor, (struct sockaddr *)&direccion, sizeof(direccion)) <
      0) {
    cout << "Error en bind: " << strerror(errno) << endl;
    throw runtime_error("Error: No se pudo asociar el socket a la direccion "
                        "IP y el puerto: " +
                        to_string(puertoUtilizado));
  }
  if (listen(socketServidor, cantUsuariosMaximo) < 0) {
    close(socketServidor);
    throw runtime_error("Error: El listen fallo.");
  }
}
void Servidor::manejadorCliente(int socketCliente) {

  MensajeServidor mensajeServidor;
  string mensajeAEnviar;
  int opcionElegida;

  mensajeServidor.partidaEnCurso = true;
  mensajeAEnviar = MensajesJuego::mensajeBienvenida.c_str();

  for (int turno = 0; turno < cantPreguntasPorPartida; turno++) {
    enviarPregunta(socketCliente, mensajeServidor,
                   preguntasSeleccionadas[turno], mensajeAEnviar);

    try {
      TransmisionMensajes::recibirMensaje(socketCliente, &opcionElegida,
                                          sizeof(int), 0);
    } catch (const exception &) {

      cout << "El cliente " << socketClienteNickname[socketCliente]
           << " se desconecto antes de finalizar la partida" << endl;
      return;
    }

    if (clienteAcerto(opcionElegida,
                      preguntasSeleccionadas[turno].getOpcionCorrecta())) {
      puntajes[socketClienteNickname[socketCliente]]++;
      mensajeAEnviar = MensajesJuego::mensajeRespuestaCorrecta;
    } else {
      mensajeAEnviar = MensajesJuego::mensajeRespuestaIncorrecta;
    }
  }

  mensajeServidor.partidaEnCurso = false;
  strcpy(mensajeServidor.mensaje, mensajeAEnviar.c_str());
  TransmisionMensajes::enviarMensaje(socketCliente, &mensajeServidor,
                                     sizeof(MensajeServidor));

  cout << "Finalizo Partida con cliente "
       << socketClienteNickname[socketCliente] << endl;
}

bool Servidor::clienteAcerto(int opcionElegida, int opcionCorrecta) const {
  return opcionElegida == opcionCorrecta ? true : false;
}

void Servidor::enviarPregunta(int sockCliente, MensajeServidor &msjServidor,
                              const Pregunta pregunta,
                              string mensajeDefinidoPorServidor) const {
  strcpy(msjServidor.pregunta, pregunta.getPregunta().c_str());

  for (int i = 0; i < CANT_OPCIONES; i++) {
    strcpy(msjServidor.opciones[i], pregunta.getOpcionSegunPosicion(i).c_str());
  }
  strcpy(msjServidor.mensaje, mensajeDefinidoPorServidor.c_str());
  TransmisionMensajes::enviarMensaje(sockCliente, &msjServidor,
                                     sizeof(MensajeServidor));
}
void Servidor::aceptarConexion() {

  int socketCliente;
  struct sockaddr_in direccionCliente;
  socklen_t tamDireccionCliente = sizeof(direccionCliente);

  // Ciclo infinito hasta que se cierre el servidor.
  while (true) {
    socketCliente = accept(socketServidor, (struct sockaddr *)&direccionCliente,
                           &tamDireccionCliente);
    if (socketCliente == -1) {

      if (errno == EBADF) {
        throw runtime_error("El servidor se cerró inesperadamente.");
      } else {
        throw runtime_error("Error al aceptar la conexión: " +
                            string(strerror(errno)));
      }
    }
    if (debeCerrarse()) {
      break;
    }
    char buffer[TAM_NICKNAME];
    TransmisionMensajes::recibirMensaje(socketCliente, buffer, sizeof(buffer),
                                        0);

    if (salaLlena()) {
      rechazarConexionPartidaEmpezada(socketCliente);
    } else {
      sacarClientesCaidos();

      string nicknameCliente(buffer);

      if (nicknameDuplicado(nicknameCliente)) {
        rechazarNicknameDuplicado(socketCliente);
      } else {
        confirmarConexion(socketCliente, nicknameCliente);
      }
    }
  }
}

bool Servidor::nicknameDuplicado(const string &nicknameCliente) const {
  return puntajes.find(nicknameCliente) != puntajes.end();
}

void Servidor::sacarClientesCaidos() {
  mutexAccesosCompartidos.lock();
  char buffer[1]; // Buffer vacío, no esperamos leer datos reales
  int socketCliente;
  // Usamos un iterador para evitar problemas al eliminar elementos
  for (auto it = socketsClientes.begin(); it != socketsClientes.end();) {
    socketCliente = *it;
    try {
      TransmisionMensajes::recibirMensaje(socketCliente, buffer, sizeof(buffer),
                                          MSG_PEEK | MSG_DONTWAIT);
      ++it;
    } catch (const exception &) {
      cout << "El cliente " << socketClienteNickname[socketCliente]
           << " se ha desconectado." << endl;
      close(socketCliente);
      it = socketsClientes.erase(it); // Eliminar cliente y avanzar al siguiente
      puntajes.erase(socketClienteNickname[socketCliente]);
      socketClienteNickname.erase(socketCliente);
      --cantidadJugadoresConectados;
    }
  }
  mutexAccesosCompartidos.unlock();
}

void Servidor::rechazarNicknameDuplicado(int socketCliente) const {
  ComunicacionEstadoConexion comunicacion;

  comunicacion.codigoEstado = COMUNICACION_NICKNAME_DUPLICADO;
  strcpy(comunicacion.mensaje,
         "Lo sentimos! El nickname ingresado ya se ha registrado "
         "por favor intente de nuevo con uno diferente.");

  TransmisionMensajes::enviarMensaje(socketCliente, &comunicacion,
                                     sizeof(comunicacion));
}

void Servidor::rechazarConexionPartidaEmpezada(int socketCliente) const {
  ComunicacionEstadoConexion comunicacion;

  comunicacion.codigoEstado = COMUNICACION_PARTIDA_EMPEZADA;
  strcpy(comunicacion.mensaje,
         "Lo sentimos! La partida ya ha empezado, intenta mas tarde.");

  TransmisionMensajes::enviarMensaje(socketCliente, &comunicacion,
                                     sizeof(comunicacion));
}

void Servidor::confirmarConexion(int socketCliente, string &nickname) {
  mutexAccesosCompartidos.lock();
  ComunicacionEstadoConexion comunicacion;

  cantidadJugadoresConectados++;
  socketsClientes.emplace_back(socketCliente);
  puntajes[nickname] = 0;
  socketClienteNickname[socketCliente] = nickname;
  comunicacion.codigoEstado = COMUNICACION_NICKNAME_EXITO;
  strcpy(comunicacion.mensaje,
         "Hemos recibido su nickname! Por favor aguarde hasta que "
         "el juego comience...");

  TransmisionMensajes::enviarMensaje(socketCliente, &comunicacion,
                                     sizeof(comunicacion));
  mutexAccesosCompartidos.unlock();
}

void Servidor::jugar() {
  mutexAccesosCompartidos.lock();
  for (int sockCliente : socketsClientes) {

    hilosClientes.emplace_back(
        [this, sockCliente]() { manejadorCliente(sockCliente); });
  }
  mutexAccesosCompartidos.unlock();
  cout << "Sala llena, iniciando juego..." << endl;
  for (thread &hilo : hilosClientes) {
    if (hilo.joinable()) {
      hilo.join();
    }
  }

  cout << "Todos los clientes han finalizado la partida." << endl;
}

void Servidor::enviarResultados() {

  sacarClientesCaidos();
  mutexAccesosCompartidos.lock();

  vector<pair<int, string>> vectorPuntajes;

  // Invertir el orden: puntaje, nickname
  for (const auto &par : puntajes) {
    vectorPuntajes.emplace_back(par.second, par.first);
  }
  // Ordenar vector de mayor puntaje a menor.
  sort(vectorPuntajes.begin(), vectorPuntajes.end(),
       [](const auto &a, const auto &b) {
         if (a.first != b.first)
           return a.first > b.first;
         return a.second < b.second;
       });

  vector<Resultado> resultados;
  Resultado resultado;

  int posicion = 1;

  // Si un jugador empata con otro en puntaje obtienen la misma posicion.
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

  // Si tengo muchos clientes, probablemente no pueda mandar todos los
  // resultados en una solo ciclo, ya que el buffer puede ser menor al tamaño
  // del mensaje total.

  // Estructura del mensaje: Cantidad resultados escritos en el buffer(int) +
  // Faltan mas resultados por pasar(bool) + Envio de resultados (Resultado[])

  int cantResultadosPosiblesEnBuffer =
      (sizeof(buffer) - sizeof(int) - sizeof(bool)) / sizeof(Resultado);

  int cantidadMensajesPorJugador =
      floor(this->cantJugadoresMaximo / cantResultadosPosiblesEnBuffer) + 1;

  int copiaCantidadJugadores = this->cantJugadoresMaximo;
  bool quedanMensajesPorEnviar;
  int i;

  for (const auto &par : socketClienteNickname) {

    // Se hace envio primero de la cantidad de jugadores conectados en la
    // partida.
    TransmisionMensajes::enviarMensaje(par.first, &(this->cantJugadoresMaximo),
                                       sizeof(int));

    quedanMensajesPorEnviar = true;
    for (i = 0; i < cantidadMensajesPorJugador - 1; i++) {
      memcpy(buffer, &cantResultadosPosiblesEnBuffer, sizeof(int));
      memcpy(buffer + sizeof(int), &quedanMensajesPorEnviar, sizeof(bool));

      copiarResultados(buffer + sizeof(int) + sizeof(bool), resultados,
                       i * cantResultadosPosiblesEnBuffer,
                       cantResultadosPosiblesEnBuffer);
      copiaCantidadJugadores -= cantResultadosPosiblesEnBuffer;
      TransmisionMensajes::enviarMensaje(par.first, buffer, sizeof(buffer));
    }
    quedanMensajesPorEnviar = false;
    memcpy(buffer, &copiaCantidadJugadores, sizeof(int));
    memcpy(buffer + sizeof(int), &quedanMensajesPorEnviar, sizeof(bool));

    copiarResultados(buffer + sizeof(int) + sizeof(bool), resultados,
                     i * cantResultadosPosiblesEnBuffer,
                     copiaCantidadJugadores);
    TransmisionMensajes::enviarMensaje(par.first, buffer, sizeof(buffer));

    cout << "Resultado enviado a: " << par.second << endl;
    copiaCantidadJugadores = this->cantJugadoresMaximo;
  }
  mutexAccesosCompartidos.unlock();
}

void Servidor::copiarResultados(char buffer[], vector<Resultado> &resultados,
                                int posDondeEmpezarACopiar,
                                int cantResultadosACopiar) const {

  for (int i = 0; i < cantResultadosACopiar; i++) {
    memcpy(buffer + sizeof(Resultado) * i,
           &resultados[posDondeEmpezarACopiar + i], sizeof(Resultado));
  }
}

void Servidor::cerrar() {
  mutexAccesosCompartidos.lock();
  finServidor = true;

  if (hiloAceptarConexiones.joinable()) {

    // Me conecto al socket del servidor solo para salir del accept() bloqueante
    // en el hilo aceptarConexiones, ya que no encuentro otra forma de levantar
    // la funcion accept().
    socketSenal = socket(AF_INET, SOCK_STREAM, 0);
    if (socketSenal < 0) {
      throw runtime_error(
          "No se pudo crear el socket para enviar senal de finalizacion.");
    }
    struct sockaddr_in direccionSenal;
    direccionSenal.sin_family = AF_INET;
    direccionSenal.sin_port = htons(puertoUtilizado);

    if (inet_pton(AF_INET, ipServidor.c_str(), &direccionSenal.sin_addr) <= 0) {
      close(socketSenal);
      throw runtime_error("Error en el inet_pton para finalizar el servidor.");
    }
    if (connect(socketSenal, (struct sockaddr *)&direccionSenal,
                sizeof(direccionSenal)) < 0) {
      close(socketSenal);
      throw runtime_error(
          "No se pudo conectar con el servidor para finalizarlo.");
    }
    close(socketSenal);

    hiloAceptarConexiones.join();
  }

  for (const auto &socket : socketsClientes) {
    close(socket);
  }
  socketsClientes.clear();

  if (socketServidor != -1) {
    close(socketServidor);
    socketServidor = -1;
  }

  if (semServidor != nullptr && semServidor != SEM_FAILED) {
    sem_close(semServidor);
    sem_unlink(NOMBRE_SEM_SERVIDOR);
    semServidor = nullptr;
  }
  mutexAccesosCompartidos.unlock();
}

int Servidor::elegirPreguntaRandom(const vector<Pregunta> &preguntas) const {

  random_device rd;  // Semilla
  mt19937 gen(rd()); // Generador de Mersenne Twister

  uniform_int_distribution<> dis(0, preguntas.size() - 1);

  return dis(gen);
}

void Servidor::cargarPreguntas(vector<Pregunta> preguntas) {

  if (preguntas.size() < (unsigned)cantPreguntasPorPartida) {
    throw runtime_error("El vector de preguntas no contiene al menos la misma "
                        "cantidad o mas de la solicitada.");
  }

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

    instanciaServidor->cerrar();
    if (instanciaServidor->salaVacia()) {
      cout << "\033[32mEl servidor se cerró exitosamente.\033[0m" << endl;
      exit(EXIT_FAILURE);
    } else {
      cout << "\033[31mEl servidor se cerró inesperadamente.\033[0m" << endl;
      exit(EXIT_SUCCESS);
    }
  }
}

void Servidor::reiniciar() {
  mutexAccesosCompartidos.lock();

  socketsClientes.clear();
  puntajes.clear();
  socketClienteNickname.clear();
  preguntasSeleccionadas.clear();
  hilosClientes.clear();
  cantidadJugadoresConectados = 0;

  mutexAccesosCompartidos.unlock();
}
