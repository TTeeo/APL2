
#pragma once

#include "file.hpp"
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

#define ARGUMENTOS_ESPERADOS 4
#define TEXTO_CONSOLA_COLOR_ROJO string("\033[31m")
#define TEXTO_CONSOLA_COLOR_AZUL string("\033[34m")
#define RESET_COLOR_CONSOLA string("\033[0m")

enum class EstadoParametro {
  Exito = 0,
  ParamArchivoNoCoincide = 1,
  ParamCantidadNoCoincide = 2,
  NoEsUnArchivoValido = 3,
  NoEsUnNumero = 4,
  NoEsUnNumeroPositivo = 5,
  NumeroFueraDeRango = 6,
  CantidadSuperaPreguntasDisponibles = 7
};

class Parametros {
private:
  string nombreArchivo;
  int cantidadPreguntas;
  bool ayudaSolicitada = false;

  void setNombreArchivo(string archivo) { this->nombreArchivo = archivo; }
  void setcantidadPreguntas(int cantidad) {
    this->cantidadPreguntas = cantidad;
  }
  bool noSeIngresaronParametros(int argc) { return argc == 0; }
  bool seSolicitoAyuda(int argc, char *argv[]);
  void procesarArgumentos(string &paramArchivo, string &archivo,
                          string &paramCantidad, string &cantidad);
  EstadoParametro validarParametroCantidad(const string &arg,
                                           const string &cant) const;
  EstadoParametro validarParametroArchivo(const string &arg,
                                          const string &dir) const;

  bool esParametroArchivo(const string arg) const {
    return !arg.compare("--archivo") || !arg.compare("-a") ? true : false;
  }
  bool esParametroCantidad(const string arg) const {
    return !arg.compare("--cantidad") || !arg.compare("-c") ? true : false;
  }
  void mostrarAyuda() const;

public:
  Parametros() = default;
  Parametros(int argc, char *argv[]);
  void mostrarParametros() {
    cout << "Archivo: " << getNombreArchivo()
         << ", Cantidad: " << getCantidadPreguntas() << endl;
  }
  string getNombreArchivo() const { return nombreArchivo; }
  int getCantidadPreguntas() const { return cantidadPreguntas; }
  bool getAyudaSolicitada() const { return ayudaSolicitada; }
};
