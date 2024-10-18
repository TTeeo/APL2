#include "question.hpp"

ostream &operator<<(ostream &os, const Pregunta &pregunta) {
  os << "Pregunta: " << pregunta.getPregunta() << "\nOpciones:\n";
  for (const auto &opcion : pregunta.getOpciones()) {
    os << "- " << opcion << "\n";
  }
  os << "Respuesta Correcta: "
     << pregunta.getOpcionSegunPosicion(pregunta.getOpcionCorrecta());
  return os;
}
