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
  static Pregunta parsearLinea(const string &linea);

public:
  static vector<Pregunta> obtenerPreguntas(string nombreArchivo);
  static int obtenerPreguntasDisponibles(string nombreArchivo);
};
