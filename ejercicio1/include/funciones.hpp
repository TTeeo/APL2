#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <termios.h>

using namespace std;

void verificarProceso(pid_t pid, string nombre);

void trabajoProceso(string nombre);

void simularTrabajo();

void crearZombie();

void crearDemonio();

void finalizarProceso(pid_t pid);

void presionarTecla();

void crearJerarquia();

void finalizarJerarquia();

void signalHandler(int signalNum);
