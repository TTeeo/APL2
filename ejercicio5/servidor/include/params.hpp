#pragma once

#include <cstring>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

#define ARGUMENTOS_ESPERADOS 8
#define USUARIOS_SOPORTADOS 10

using namespace std;

class ExcepcionAyuda : public exception {
private:
  string mensajeAyuda =
      "Uso: servidor [opciones]\nOpciones:\n"
      "   -p / --puerto      Número de puerto (Requerido).\n"
      "   -u / --usuarios    Cantidad de usuarios a esperar para iniciar la "
      "sala. (Requerido).\n"
      "   -a / --archivo     Archivo con las preguntas (Requerido).\n"
      "   -c / --cantidad    Cantidad de preguntas por partida (Requerido).\n"
      "   -h / --help       Muestra la ayuda del ejercicio.\n"
      "Ejemplo de uso del servidor:\n"
      "  ./bin/servidor.exe -p 8080 -u 3 -a LoteDePrueba/preguntas.txt -c 3\n"
      "  ./bin/servidor.exe --puerto 8080 --usuarios 3 --archivo "
      "LoteDePrueba/preguntas.txt "
      "--cantidad 3\n"
      "Consideraciones:\n"
      "   -Solo soportamos la cantidad de 10 usuarios jugando "
      "simultaneamente.\n"
      "   -Para cerrar el servidor puede usar la senal SIGUSR1.\n";

public:
  ExcepcionAyuda() = default;
  const char *what() const noexcept override { return mensajeAyuda.c_str(); }
};

class Parametros {

private:
  map<string, function<void(const char *)>> opciones;
  int puerto;
  int cantPreguntasPorPartida;
  int cantUsuarios;
  string archivoPreguntas;

  void inicializarOpciones();

  void ejecutarOpcion(string clave, const char *arg);
  void opcionPuerto(string opcion);
  void opcionUsuarios(string opcion);
  void opcionCantidad(string opcion);
  void opcionArchivo(string opcion);
  bool seSolicitoAyuda(int argc, const char *opcion) const {
    return argc == 1 && (!strcmp(opcion, "-h") || !strcmp(opcion, "--help"));
  }

public:
  Parametros() = default;
  Parametros(int argc, const char *argv[]);
  int getCantidadPreguntas() const { return cantPreguntasPorPartida; }
  int getPuerto() const { return puerto; }
  int getCantidadUsuarios() const { return cantUsuarios; }
  string getNombreArchivoPreguntas() const { return archivoPreguntas; }
  string getArchivoPreguntas() const { return archivoPreguntas; }
};