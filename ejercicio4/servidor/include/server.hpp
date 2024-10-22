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
  sem_t *semServidor = nullptr, *semCliente = nullptr;
  int shmId = -1;
  DatosCompartidos *datos = (DatosCompartidos *)-1;

  const vector<Pregunta> preguntas;
  vector<Pregunta> copiaPreguntas;
  int cantPreguntasPorPartida;
  static bool ordenFinDeServidor;
  static Servidor *instaciaServidor;
  // static void manejarSIGUSR1(int signal);
  // static void ignorarSIGINT([[maybe_unused]] int signal);

  key_t crearClaveParaMemCompartida();
  void liberarSemaforos();
  Pregunta elegirPreguntaRandom();
  void enviar() { sem_post(this->semServidor); }
  void esperar() { sem_wait(this->semCliente); }
  void inicializarDatosCompartidos();
  bool clienteAcerto(Pregunta &preg);
  void escribirPid() const;

public:
  Servidor(vector<Pregunta> pregs, int cantPreguntas)
      : preguntas(pregs), cantPreguntasPorPartida(cantPreguntas){};
  void iniciar();
  void jugar();
  static void manejadorSenalFinServidor(int signo);
  static void manejadorConexionCliente(int signo) { (void)signo; }
  bool cerrar() const { return Servidor::ordenFinDeServidor; }
  void liberarRecursos();

  ~Servidor() { liberarRecursos(); }
};
