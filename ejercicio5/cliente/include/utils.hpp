#pragma once

#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>

#define DATOS_NO_DISPONIBLES 0

class TransmisionMensajes {

public:
  static void enviarMensaje(int socketCliente, const void *dato,
                            size_t tamDato);
  static ssize_t recibirMensaje(int socketCliente, void *buffer,
                                size_t tamBuffer, int configRecv);
};
