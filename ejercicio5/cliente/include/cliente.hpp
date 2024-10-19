#pragma once

#include "config.hpp"
#include <arpa/inet.h>
#include <csignal>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unistd.h>

using namespace std;

class Cliente {

private:
  string nickname = "";
  int descriptorSocket = -1;
  static Cliente *instanciaCliente;

  void MostrarPregunta(MensajeServidor &msjServidor) const;

  int obtenerRespuestaCliente() const;

  bool esOpcionValida(int opcion) const;
  void mostrarResultadoJugador(Resultado &res) const;

public:
  Cliente(string nombre);
  static void manejadorFinCliente(int signo);
  void crearSocket(string ip, int puerto, string nickname);
  void jugar();
  void cerrarSocket();
  bool juegoListoParaIniciar() const;
  ~Cliente();
};
