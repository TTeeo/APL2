#include "client.hpp"

Cliente *Cliente::instanciaCliente = nullptr;

void Cliente::iniciar() {

  if (!filesystem::exists(PATH_SHMFILE)) {
    throw runtime_error("Error: No hay un servidor ejecutándose.");
  }

  this->semBloqueoClientes =
      sem_open(this->NOMBRE_SEM_ENTRE_CLIENTES, O_CREAT | O_EXCL, 0666, 0);
  if (this->semBloqueoClientes == SEM_FAILED) {
    if (errno == EEXIST) {
      throw runtime_error("Error: Ya hay otro cliente ejecutándose.");
    } else {
      throw runtime_error(
          "Error: No se pudo crear el semaforo para bloquear clientes.");
    }
  }
  this->semCliente = sem_open(NOMBRE_SEM_CLIENTE.c_str(), 0);
  this->semServidor = sem_open(NOMBRE_SEM_SERVIDOR.c_str(), 0);

  if (this->semCliente == SEM_FAILED || this->semServidor == SEM_FAILED) {
    throw runtime_error(
        "Error: No se pudo abrir el semaforo que conecta con el Servidor.");
  }

  key_t clave = crearClaveParaMemCompartida();
  this->shmId = shmget(clave, sizeof(DatosCompartidos), IPC_CREAT | 0666);
  if (this->shmId == -1) {
    throw runtime_error("Error: No se pudo obtener la memoria compartida.");
  }
  this->datos = static_cast<DatosCompartidos *>(shmat(shmId, nullptr, 0));
  if (this->datos == (DatosCompartidos *)-1) {
    throw runtime_error("Error: No se pudo adjuntar la memoria compartida.");
  }
  instanciaCliente = this;
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  signal(SIGUSR1, Cliente::manejadorCierreInesperado);

  despertarServidor();
}

void Cliente::despertarServidor() const {

  if (filesystem::exists(nombreArchPidServidor)) {
    ifstream archivoPid(nombreArchPidServidor);
    pid_t pidServidor;

    if (archivoPid.is_open()) {
      archivoPid >> pidServidor;
      archivoPid.close();
    } else {
      throw runtime_error(
          "No se pudo abrir el archivo con el PID del servidor.");
    }
    if (kill(pidServidor, SIGUSR2) != 0) {
      throw runtime_error("Error al enviar la señal al servidor.");
    }
  } else {
    throw runtime_error(
        "No se pudo obtener el archivo con el PID del servidor.");
  }
}

void Cliente::manejadorCierreInesperado(int signo) {

  if (SIGUSR1 == signo) {
    if (instanciaCliente != nullptr)
      instanciaCliente->cerrarCliente = true;
  }
}

void Cliente::liberarSemaforos() {
  if (this->semCliente != nullptr && this->semCliente != SEM_FAILED) {
    sem_close(this->semCliente);
  }

  if (this->semServidor != nullptr && this->semServidor != SEM_FAILED) {
    sem_close(this->semServidor);
  }
  if (this->semBloqueoClientes != nullptr &&
      this->semBloqueoClientes != SEM_FAILED) {
    sem_close(this->semBloqueoClientes);
    sem_unlink(this->NOMBRE_SEM_ENTRE_CLIENTES);
  }
}

void Cliente::mostrarPreguntaYOpciones() const {
  cout << this->datos->pregunta << "\n" << endl;
  cout << this->datos->opciones[0] << endl;
  cout << this->datos->opciones[1] << endl;
  cout << this->datos->opciones[2] << endl;
}
int Cliente::obtenerRespuestaCliente() const {

  string respuesta;
  int numeroOpcion = -1;

  while (!esOpcionValida(numeroOpcion)) {
    cout << "\nIngrese la opcion correcta [1-3]: ";
    cin >> respuesta;

    try {
      numeroOpcion = stoi(respuesta);

      if (!esOpcionValida(numeroOpcion)) {
        cout << "La opcion ingresada no es valida. Por favor, vuelva a "
                "intentar."
             << endl;
      }
    } catch (const exception &) {
      cout << "La opcion ingresada no es un numero. Por favor, vuelva a "
              "intentar."
           << endl;
    }
  }
  return numeroOpcion;
}

bool Cliente::esOpcionValida(int opcion) const {
  return opcion > 0 && opcion <= CANT_OPCIONES ? true : false;
}
int Cliente::jugar() const {
  enviar();
  int respuesta;

  this->datos->partidaEnCurso = true;
  this->datos->cierreInesperado = false;

  while (this->datos->partidaEnCurso && !this->datos->cierreInesperado) {
    esperar();

    if (!this->datos->partidaEnCurso) {
      break;
    }
    cout << this->datos->mensajeServidor << endl;
    mostrarPreguntaYOpciones();
    respuesta = obtenerRespuestaCliente();
    this->datos->respuestaCliente = respuesta;

    if (cerrar()) {
      this->datos->cierreInesperado = true;
    }
    enviar();
  }

  if (this->datos->cierreInesperado) {
    throw runtime_error("El cliente se ha desconectado durante la partida.");
  }
  cout << this->datos->mensajeServidor << endl;
  return this->datos->puntajeFinal;
}

key_t Cliente::crearClaveParaMemCompartida() {

  ofstream shmFile;
  shmFile.open(PATH_SHMFILE);

  if (!shmFile) {
    throw runtime_error("Error: No se pudo abrir el archivo SHMFILE.");
  }
  shmFile.close();
  return ftok(PATH_SHMFILE.c_str(), PROJECT_ID);
}