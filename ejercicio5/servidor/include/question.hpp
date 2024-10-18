#pragma once

#include <array>
#include <iostream>
#include <string>
#include <vector>

#define CANT_OPCIONES 3

using namespace std;

class Pregunta {

private:
  string pregunta;
  int opcionCorrecta;
  string opciones[CANT_OPCIONES];

public:
  Pregunta() = default;
  Pregunta(string texto, int respuesta, string *opts)
      : pregunta(texto), opcionCorrecta(respuesta) {

    setOpciones(opts);
  }

  void setPregunta(const string pregunta) { this->pregunta = pregunta; }
  void setOpcionCorrecta(int opt) { this->opcionCorrecta = opt; }
  void setOpciones(const string *opts) {
    for (int i = 0; i < CANT_OPCIONES; i++) {
      this->opciones[i] = opts[i];
    }
  }

  string getPregunta() const { return this->pregunta; }
  int getOpcionCorrecta() const { return this->opcionCorrecta; }
  vector<string> getOpciones() const {

    vector<string> opciones;
    opciones.emplace_back(getOpcionSegunPosicion(0));
    opciones.emplace_back(getOpcionSegunPosicion(1));
    opciones.emplace_back(getOpcionSegunPosicion(2));

    return opciones;
  }
  string getOpcionSegunPosicion(int pos) const { return this->opciones[pos]; }

  void mostrarPregunta(const Pregunta &p) const {
    cout << "Pregunta: " << p.pregunta << endl; // Muestra la pregunta
    cout << "Opciones:" << endl;

    // Itera y muestra cada opción
    for (int i = 0; i < CANT_OPCIONES; ++i) {
      cout << i + 1 << ". " << p.opciones[i]
           << endl; // Muestra el índice (1-based) y la opción
    }

    cout << "Seleccione la opción correcta (1-" << CANT_OPCIONES << "): ";
  }
};
ostream &operator<<(std::ostream &os, const Pregunta &pregunta);