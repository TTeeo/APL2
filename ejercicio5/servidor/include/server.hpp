#pragma once

#include "config.hpp"
#include "message.hpp"
#include "question.hpp"
#include "utils.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <atomic>
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <future>
#include <iostream>
#include <limits>
#include <map>
#include <mutex>
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
  const int puertoUtilizado;
  const int cantJugadoresMaximo;
  const int cantPreguntasPorPartida;
  const string ipServidor = "127.0.0.1";

  sem_t *semServidor = nullptr;
  int socketServidor = -1;
  int socketSenal =
      -1; // Su funcion es destrabar el accept() en el hilo aceptarConexiones.
  int cantidadJugadoresConectados = 0;
  vector<int> socketsClientes;
  vector<thread> hilosClientes;
  vector<Pregunta> preguntasSeleccionadas;
  map<string, int> puntajes;
  map<int, string> socketClienteNickname;
  thread hiloAceptarConexiones;
  static Servidor *instanciaServidor;
  atomic<bool> finServidor = false;
  mutex mutexAccesosCompartidos;

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
  void aceptarConexion();
  void sacarClientesCaidos();
  void confirmarConexion(int socketCliente, string &nickname);
  bool nicknameDuplicado(const string &nicknameCliente) const;
  void rechazarConexionPartidaEmpezada(int socketCliente) const;
  void rechazarNicknameDuplicado(int socketCliente) const;
  bool salaVacia() const { return cantidadJugadoresConectados == 0; }
  bool partidaEmpezada() const {
    return (int)puntajes.size() ==
           cantJugadoresMaximo; // Utilizo puntajes ya que socketsClientes puede
                                // < cantJugadoresMaximo debido a las
                                // desconexiones.
  }

public:
  Servidor(int puerto, int cantJugadores, int cantPreguntas);
  void crearSocket(int cantUsuariosMaximo);

  void aceptarConexiones() {
    hiloAceptarConexiones = thread(&Servidor::aceptarConexion, this);
  }

  static void manejadorFinDeServidor(int signo);

  void jugar();
  bool salaLlena() const {
    return cantidadJugadoresConectados == cantJugadoresMaximo;
  }
  vector<Pregunta> getPreguntas() const { return preguntasSeleccionadas; }
  void cargarPreguntas(vector<Pregunta> preguntas);
  void enviarResultados();
  void reiniciar();
  bool debeCerrarse() const { return finServidor; }
  void cerrar();
  ~Servidor() { cerrar(); }
};
