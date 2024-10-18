#pragma once

#include "question.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

class Archivo {

private:
  vector<Pregunta> preguntas;
  int preguntasTotales = 0;

public:
  Archivo() = default;
  Archivo(const string nombre);
  bool preguntasInsuficientes(int cant) { return cant > preguntasTotales; }
  vector<Pregunta> getPreguntas() const { return preguntas; }
};