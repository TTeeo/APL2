#pragma once

#include "config.hpp"
#include "message.hpp"
#include "question.hpp"
#include <algorithm>
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <limits>
#include <map>
#include <netinet/in.h>
#include <random>
#include <semaphore.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <utility>
#include <vector>

#define NOMBRE_SEM_SERVIDOR "/semaforo_servidor"
using namespace std;

class Servidor {
private:
  sem_t *semServidor = nullptr;
  int socketServidor = -1;
  int cantidadJugadoresConectados = 0;
  vector<int> socketsClientes;
  vector<thread> hilosClientes;
  const int cantJugadoresMaximo;
  const int cantPreguntasPorPartida;
  vector<Pregunta> preguntasSeleccionadas;
  map<string, int> puntajes;
  map<int, string> socketClienteNickname;
  thread rechazarClientes;
  static Servidor *instanciaServidor;

  void manejadorCliente(int sockCliente);
  string procesarRespuestaCliente(MensajeCliente &msjCliente,
                                  int opcionCorrecta);
  void enviarPregunta(int sockCliente, MensajeServidor &msjServidor,
                      const Pregunta pregunta,
                      string mensajeDefinidoPorServidor) const;
  string obtenerNickname(const char *const buffer) const;
  int elegirPreguntaRandom(const vector<Pregunta> &preguntas) const;
  string obtenerNicknameCliente(int socketCliente) const;
  void rechazarConexiones();
  bool nicknameDuplicado(const string &nicknameCliente) const;

public:
  Servidor() = default;
  Servidor(int cantJugadores, int cantPreguntas);
  void crearSocket(int puerto, int cantUsuariosMaximo);
  void aceptarConexionNueva();
  static void manejadorFinDeServidor(int signo);
  void sacarClientesCaidos();
  void jugar();
  bool salaLlena() const {
    return cantidadJugadoresConectados == cantJugadoresMaximo;
  }
  vector<Pregunta> getPreguntas() const { return preguntasSeleccionadas; }
  void cargarPreguntas(vector<Pregunta> preguntas);
  void cargarPreguntas(const string nombreArchivo);
  void enviarResultados() const;
  void liberarRecursos() const;
  void confirmarPartida();
  void reiniciar();
  // void mostrarJugadoresConectados() const;
  int getCantJugadores() { return cantidadJugadoresConectados; }
  ~Servidor();
};
