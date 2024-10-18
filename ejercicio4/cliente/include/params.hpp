#pragma once
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

#define ARGUMENTOS_ESPERADOS 2
#define TEXTO_CONSOLA_COLOR_ROJO string("\033[31m")
#define TEXTO_CONSOLA_COLOR_AZUL string("\033[34m")
#define RESET_COLOR_CONSOLA string("\033[0m")

class Parametros {

private:
  string nombreUsuario;
  bool ayudaSolicitada = false;

public:
  bool getAyudaSolicitada() const { return ayudaSolicitada; }

  void mostrarAyuda() const;
  Parametros() = default;
  Parametros(int argc, const char *argv[]);

  string getNombreUsuario() const { return this->nombreUsuario; }
};