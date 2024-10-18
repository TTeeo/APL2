#include "file.hpp"

Archivo::Archivo(const string nombre) {
  ifstream archivo(nombre);

  if (!archivo.is_open()) {
    throw invalid_argument("No se pudo abrir el archivo");
  }

  string linea;

  while (getline(archivo, linea)) {
    stringstream ss(linea);
    string item;
    vector<string> elementos;

    while (getline(ss, item, ',')) {
      elementos.push_back(item);
    }
    preguntasTotales++;
    if (elementos.size() != 5) {
      throw invalid_argument("Error: La linea " + to_string(preguntasTotales) +
                             " no pudo ser parseada");
    }
    string opciones[CANT_OPCIONES];
    copy(elementos.begin() + 2, elementos.begin() + 2 + CANT_OPCIONES,
         opciones);

    preguntas.push_back(Pregunta(elementos[0], stoi(elementos[1]), opciones));
  }

  archivo.close();
}
