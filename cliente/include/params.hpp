#pragma once

#include <cstdlib>
#include <getopt.h> // Para getopt_long
#include <iostream>
#include <string>

#define CANT_ARGS_AYUDA 2
#define CANT_ARGS_ESPERADOS 7

using namespace std;

class Parametros {

private:
  string nickname;
  string servidorIp;
  int puerto;

public:
  void mostrarAyuda() const;
  string getNickname() const { return nickname; }
  string getServidorIp() const { return servidorIp; }
  int getPuerto() const { return puerto; }
  Parametros(int argc, char *argv[]);
};