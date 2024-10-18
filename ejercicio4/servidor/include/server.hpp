#pragma once

#include "config.hpp"
#include "question.hpp"
#include <csignal>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <semaphore.h>
#include <stdexcept>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

using namespace std;

class Servidor {

private:
  sem_t *semServidor = SEM_FAILED, *semCliente = SEM_FAILED;
  int shmId = -1;
  DatosCompartidos *datos = (DatosCompartidos *)-1;

  vector<Pregunta> preguntas;
  int cantPreguntasPorPartida;
  static bool clienteJugando;
  static bool ordenFinDeServidor;
  static Servidor *instaciaServidor;
  // static void manejarSIGUSR1(int signal);
  // static void ignorarSIGINT([[maybe_unused]] int signal);

  key_t crearClaveParaMemCompartida();
  void liberarRecursos();
  void liberarSemaforos();
  Pregunta elegirPreguntaRandom();
  void enviar() { sem_post(this->semServidor); }
  void esperar() { sem_wait(this->semCliente); }

public:
  Servidor(vector<Pregunta> pregs, int cantPreguntas)
      : preguntas(pregs), cantPreguntasPorPartida(cantPreguntas){};
  void iniciar();
  void jugar();
  static void manejadorSenalFinServidor(int signo);
  bool cerrar() const { return Servidor::ordenFinDeServidor; }

  ~Servidor() { liberarRecursos(); }
};
