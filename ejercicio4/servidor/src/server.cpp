#include "server.hpp"

bool Servidor::ordenFinDeServidor = false;
Servidor *Servidor::instaciaServidor = nullptr;

void Servidor::iniciar() {

  this->semServidor =
      sem_open(NOMBRE_SEM_SERVIDOR.c_str(), O_CREAT | O_EXCL, 0666, 0);
  this->semCliente =
      sem_open(NOMBRE_SEM_CLIENTE.c_str(), O_CREAT | O_EXCL, 0666, 0);

  if (this->semServidor == SEM_FAILED) {
    if (errno == EEXIST) {
      throw runtime_error("Error: Ya hay otro servidor ejecutándose.");
    } else {
      throw runtime_error("Error: No se pudo crear el semáforo del servidor.");
    }
  }

  if (this->semCliente == SEM_FAILED) {
    throw runtime_error("Error: No se pudo crear el semáforo del cliente.");
  }

  key_t clave = crearClaveParaMemCompartida();
  this->shmId = shmget(clave, sizeof(DatosCompartidos), IPC_CREAT | 0666);

  if (shmId < 0) {
    throw runtime_error(
        "Error: No se pudo crear el area de memoria compartida.");
  }

  this->datos = static_cast<DatosCompartidos *>(shmat(shmId, nullptr, 0));
  if (this->datos == (DatosCompartidos *)-1) {
    throw runtime_error(
        "Error: No se pudo adjuntar el segmento de memoria compartda.");
  }

  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  signal(SIGUSR1, Servidor::manejadorSenalFinServidor);
  signal(SIGUSR2, Servidor::manejadorConexionCliente);
  this->instaciaServidor = this;

  escribirPid();
}

void Servidor::escribirPid() const {

  ofstream archivoPid(nombreArchPidServidor);
  if (!archivoPid) {
    throw runtime_error("Error al crear el archivo temporal.");
  }
  archivoPid << getpid();
  archivoPid.close();
}
void Servidor::liberarRecursos() {

  if (this->datos != (DatosCompartidos *)-1) {
    shmdt(this->datos); // Desasociar segmento de memoria
    this->datos = (DatosCompartidos *)-1;
  }
  if (this->shmId >= 0) {
    shmctl(shmId, IPC_RMID, nullptr); // Liberar memoria compartida
    this->shmId = -1;
  }

  if (filesystem::exists(PATH_SHMFILE)) {
    filesystem::remove(PATH_SHMFILE);
  }
  if (filesystem::exists(nombreArchPidServidor)) {
    filesystem::remove(nombreArchPidServidor);
  }

  liberarSemaforos();
}

void Servidor::liberarSemaforos() {

  if (this->semServidor != nullptr && this->semServidor != SEM_FAILED) {
    sem_close(this->semServidor);
    sem_unlink(NOMBRE_SEM_SERVIDOR.c_str());
    this->semServidor = nullptr;
  }
  if (this->semCliente != nullptr && this->semCliente != SEM_FAILED) {
    sem_close(this->semCliente);
    sem_unlink(NOMBRE_SEM_CLIENTE.c_str());
    this->semCliente = nullptr;
  }
}

key_t Servidor::crearClaveParaMemCompartida() {

  ofstream shmFile;
  shmFile.open(PATH_SHMFILE);

  if (!shmFile) {
    throw runtime_error("Error: No se pudo generar el archivo SHMFILE.");
  }
  shmFile.close();
  return ftok(PATH_SHMFILE.c_str(), PROJECT_ID);
}
Pregunta Servidor::elegirPreguntaRandom() {

  random_device rd;  // Semilla
  mt19937 gen(rd()); // Generador de Mersenne Twister

  uniform_int_distribution<> dis(0, this->copiaPreguntas.size() - 1);

  int indice = dis(gen);
  Pregunta preguntaEliminada = this->copiaPreguntas[indice];
  this->copiaPreguntas.erase(this->copiaPreguntas.begin() + indice);
  return preguntaEliminada;
}

void Servidor::inicializarDatosCompartidos() {
  if (this->datos != (DatosCompartidos *)-1) {
    this->datos->partidaEnCurso = true;
    this->datos->cierreInesperado = false;
    strcpy(this->datos->mensajeServidor, "Bienvenido a Preguntados! \n");
    this->datos->respuestaCliente = 0;
    this->copiaPreguntas = preguntas;
  }
}

bool Servidor::clienteAcerto(Pregunta &preg) {

  if (this->datos->respuestaCliente == preg.getOpcionCorrecta()) {
    strcpy(this->datos->mensajeServidor, "Respuesta CORRECTA! Sigue asi! \n");
    return true;
  } else {
    strcpy(this->datos->mensajeServidor, "Respuesta INCORRECTA! :( \n");
    return false;
  }
}

void Servidor::jugar() {
  esperar();

  int puntaje = 0;
  Pregunta preg;

  inicializarDatosCompartidos();

  for (int turno = 0; turno < this->cantPreguntasPorPartida; turno++) {

    if (this->datos->cierreInesperado) {
      break;
    }
    if (turno && clienteAcerto(preg)) {
      puntaje++;
    }

    preg = elegirPreguntaRandom();
    strcpy(this->datos->pregunta, preg.getPregunta().c_str());

    for (int i = 0; i < CANT_OPCIONES; i++) {
      strcpy(this->datos->opciones[i], preg.getOpcionSegunPosicion(i).c_str());
    }
    enviar();
    esperar();
  }
  if (this->datos->cierreInesperado) {
    throw runtime_error("El cliente se ha cerrado inesperadamente.\n");
  }
  if (clienteAcerto(preg)) {
    puntaje++;
  }
  this->datos->partidaEnCurso = false;
  this->datos->puntajeFinal = puntaje;
  enviar();
}

void Servidor::manejadorSenalFinServidor(int signo) {
  if (signo == SIGUSR1) {
    Servidor::ordenFinDeServidor = true;
  }
}
