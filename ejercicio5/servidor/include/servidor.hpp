#pragma once

#include "config.hpp"
#include "message.hpp"
#include "question.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <future>
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
#define CONEXION_RECHAZADA -1

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

  static Servidor *instanciaServidor;

  void rechazarConexiones();
  void manejadorCliente(int sockCliente);
  bool clienteAcerto(int opcionElegida, int opcionCorrecta) const;
  void enviarPregunta(int sockCliente, MensajeServidor &msjServidor,
                      const Pregunta pregunta,
                      string mensajeDefinidoPorServidor) const;

  int elegirPreguntaRandom(const vector<Pregunta> &preguntas) const;
  int calcularIteracionesParaEnviarResultados(size_t tamBuffer) const;
  void copiarResultados(char buffer[], vector<Resultado> &resultados,
                        int posDondeEmpezar, int cantResultadosACopiar) const;

public:
  Servidor() = default;
  Servidor(int cantJugadores, int cantPreguntas);
  void crearSocket(int puerto, int cantUsuariosMaximo);
  // probar con const
  int aceptarConexion();
  static void manejadorFinDeServidor(int signo);
  void sacarClientesCaidos();
  string obtenerNickname(int socketCliente) const;
  void rechazarNicknameDuplicado(int socketCliente) const;

  void confirmarConexion(int socketCliente, string &nickname);
  bool nicknameDuplicado(const string &nicknameCliente) const;
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
  void mostrarJugadoresConectados() const;
  int getCantJugadores() const { return cantidadJugadoresConectados; }

  ~Servidor();
};
