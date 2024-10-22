#pragma once

#include "question.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

class Archivo {

private:
  static string rutaArchivo;
  static vector<Pregunta> preguntas;

  static Pregunta parsearLinea(const string &linea);
  static bool preguntasSuficientes(string nombreArchivo);

public:
  static vector<Pregunta> obtenerPreguntas(string rutaArchParam);
  static bool preguntasInsuficientes(string nombreArchivo,
                                     int preguntasIngresadas);
  static int getCantidadPreguntas() {
    return rutaArchivo == "" ? 0 : (int)preguntas.size();
  }
};
