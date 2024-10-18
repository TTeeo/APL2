#include "server.hpp"

bool Servidor::ordenFinDeServidor = false;
bool Servidor::clienteJugando = false;
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
  signal(SIGTERM, SIG_IGN); // se ignoran el resto de señales porque se cierra con SIGUSR1
  signal(SIGUSR1, Servidor::manejadorSenalFinServidor);
  this->instaciaServidor = this;
}

void Servidor::liberarRecursos() {

  cout << "Cerrando servidor..." << endl;

  if (this->datos != (DatosCompartidos *)-1) {
    shmdt(this->datos); // Desasociar segmento de memoria
  }
  if (this->shmId >= 0) {
    shmctl(shmId, IPC_RMID, nullptr); // Liberar memoria compartida
  }

  if (filesystem::exists(PATH_SHMFILE)) {
    filesystem::remove(PATH_SHMFILE);
  }

  liberarSemaforos();
}

void Servidor::liberarSemaforos() {

  if (this->semServidor != SEM_FAILED) {
    sem_close(this->semServidor);
    sem_unlink(NOMBRE_SEM_SERVIDOR.c_str());
  }
  if (this->semCliente != SEM_FAILED) {
    sem_close(this->semCliente);
    sem_unlink(NOMBRE_SEM_CLIENTE.c_str());
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

  uniform_int_distribution<> dis(0, this->preguntas.size() - 1);

  int indice = dis(gen);
  Pregunta preguntaEliminada = this->preguntas[indice];
  this->preguntas.erase(this->preguntas.begin() + indice);
  return preguntaEliminada;
}

void Servidor::jugar() {
  esperar();
  this->clienteJugando = true;
  int puntaje = 0;
  Pregunta preg;
  this->datos->partidaEnCurso = true;

  for (int turno = 0; turno < this->cantPreguntasPorPartida; turno++) {

    if (turno == 0) {
      strcpy(this->datos->mensajeServidor, "Bienvenido a Preguntados! \n");
    } else {
      if (this->datos->respuestaCliente == preg.getOpcionCorrecta()) {
        strcpy(this->datos->mensajeServidor, "Respuesta CORRECTA! Sigue asi! \n");
        puntaje++;
      } else {
        strcpy(this->datos->mensajeServidor, "Respuesta INCORRECTA! :( \n");
      }
    }

    preg = elegirPreguntaRandom();
    strcpy(this->datos->pregunta, preg.getPregunta().c_str());

    for (int i = 0; i < CANT_OPCIONES; i++) {
      strcpy(this->datos->opciones[i], preg.getOpcionSegunPosicion(i).c_str());
    }
    enviar();
    esperar();
  }

  if (this->datos->respuestaCliente == preg.getOpcionCorrecta()) {
    strcpy(this->datos->mensajeServidor, "Respuesta CORRECTA! Sigue asi! \n");
    puntaje++;
  } else {
    strcpy(this->datos->mensajeServidor, "Respuesta INCORRECTA! :( \n");
  }

  this->datos->partidaEnCurso = false;
  this->datos->puntajeFinal = puntaje;
  enviar();
  this->clienteJugando = false;
}

void Servidor::manejadorSenalFinServidor(int signo) {
  if (signo == SIGUSR1) {
    if (Servidor::clienteJugando) {
      Servidor::ordenFinDeServidor = true;
    } else {
      instaciaServidor->liberarRecursos();
      exit(EXIT_SUCCESS);
    }
  }
}
