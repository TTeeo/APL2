#include "file.hpp"

string Archivo::rutaArchivo = "";
vector<Pregunta> Archivo::preguntas;

vector<Pregunta> Archivo::obtenerPreguntas(string rutaArchParam) {

  if (rutaArchParam == rutaArchivo) {
    return preguntas;
  }

  filesystem::path ruta(rutaArchParam);
  ifstream archivo(ruta);

  if (!archivo.is_open()) {
    throw runtime_error("Error: No se pudo abrir el archivo " +
                        ruta.filename().string());
  }

  Pregunta preg;
  string linea;

  while (getline(archivo, linea)) {
    try {
      preg = parsearLinea(linea);
      preguntas.emplace_back(preg);

    } catch (exception &e) {
      throw runtime_error("Error: No se pudo parsear la linea " + linea);
    }
  }

  rutaArchivo = rutaArchParam;
  return preguntas;
}

Pregunta Archivo::parsearLinea(const string &linea) {

  stringstream ss(linea); // Crear un stringstream para parsear la línea
  string preg, optCorrecta, opts[3];

  getline(ss, preg, ',');
  getline(ss, optCorrecta, ',');
  getline(ss, opts[0], ',');
  getline(ss, opts[1], ',');
  getline(ss, opts[2], ',');

  return Pregunta(preg, stoi(optCorrecta), opts);
}

bool Archivo::preguntasInsuficientes(string rutaArchParam,
                                     int preguntasIngresadas) {

  if (rutaArchParam != rutaArchivo) {
    obtenerPreguntas(rutaArchParam);
  }
  return (int)preguntas.size() > preguntasIngresadas ? true : false;
}