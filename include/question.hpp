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
  array<string, 3> getOpciones() const {
    return array<string, CANT_OPCIONES>{getOpcionSegunPosicion(0),
                                        getOpcionSegunPosicion(1),
                                        getOpcionSegunPosicion(2)};
  }
  string getOpcionSegunPosicion(int pos) const { return this->opciones[pos]; }
  friend ostream &operator<<(std::ostream &os, const Pregunta &pregunta);
};
