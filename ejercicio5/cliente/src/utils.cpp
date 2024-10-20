#include "utils.hpp"

void TransmisionMensajes::enviarMensaje(int socketCliente, const void *dato,
                                        size_t tamDato) {
  if (send(socketCliente, dato, tamDato, 0) == -1) {
    throw std::runtime_error(
        "Error: No se pudo enviar el mensaje al servidor.");
  }
}

ssize_t TransmisionMensajes::recibirMensaje(int socketCliente, void *buffer,
                                            size_t tamBuffer, int configRecv) {

  ssize_t bytesRecibidos;
  bytesRecibidos = recv(socketCliente, buffer, tamBuffer, configRecv);

  if (bytesRecibidos == 0) {
    throw std::runtime_error("Conexión cerrada por parte del servidor.");
  }

  if (bytesRecibidos == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return DATOS_NO_DISPONIBLES; // No hay datos disponibles
    } else {
      throw std::runtime_error("Error en la conexión con el servidor: " +
                               std::string(strerror(errno)));
    }
  }

  if (static_cast<size_t>(bytesRecibidos) > tamBuffer) {
    throw std::runtime_error(
        "Error: se han recibido más bytes de los que se esperaban.");
  }

  return bytesRecibidos;
}