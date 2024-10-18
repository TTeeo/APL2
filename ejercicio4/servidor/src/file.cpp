#include "file.hpp"

vector<Pregunta> Archivo::obtenerPreguntas(string rutaArchivo) {

  filesystem::path ruta(rutaArchivo);
  ifstream archivo(ruta);

  if (!archivo.is_open()) {
    throw runtime_error("Error: No se pudo abrir el archivo " +
                        ruta.filename().string());
  }
  vector<Pregunta> preguntas;
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

int Archivo::obtenerPreguntasDisponibles(string nombreArchivo) {

  ifstream archivo(nombreArchivo);

  if (!archivo.is_open()) {
    throw runtime_error("Error: No se pudo abrir el archivo " + nombreArchivo);
  }

  int contador = 0;
  string linea;

  while (getline(archivo, linea)) {
    contador++;
  }

  archivo.close();

  return contador;
}