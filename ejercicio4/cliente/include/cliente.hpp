#pragma once

#include "config.hpp"
#include <cerrno>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <semaphore.h>
#include <signal.h>
#include <stdexcept>
#include <string>
#include <sys/ipc.h>
#include <sys/shm.h>

using namespace std;

class Cliente {

private:
  const char *NOMBRE_SEM_ENTRE_CLIENTES = "/semaforo_entre_clientes";
  sem_t *semServidor = SEM_FAILED, *semCliente = SEM_FAILED,
        *semBloqueoClientes = SEM_FAILED;
  int shmId = -1;
  DatosCompartidos *datos = (DatosCompartidos *)-1;
  void mostrarPreguntaYOpciones() const;
  void liberarSemaforos();
  int obtenerRespuestaCliente() const;
  bool esOpcionValida(int opcion) const;

  int enviar() const { return sem_post(this->semCliente); }
  int esperar() const { return sem_wait(this->semServidor); }
  key_t crearClaveParaMemCompartida();

public:
  Cliente() = default;
  void iniciar();
  int jugar() const;
  ~Cliente() { liberarSemaforos(); }
};