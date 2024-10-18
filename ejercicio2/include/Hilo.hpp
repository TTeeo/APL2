#include <params.hpp>

#include <thread>
#include <vector>
#include <mutex>
#include <csignal>
#include <atomic>

#include <fstream>

void crearHilos(Parametros parametros);

void funcionHilo(int id, Cadena cadena);

void buscarPalabra(Archivo archivo, int id, Cadena cadena);
