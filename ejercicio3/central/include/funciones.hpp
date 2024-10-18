#include <params.hpp>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <csignal>
#include <ctime>
#include <cstring>
#include <sys/stat.h>

void crearDemonio();

void signalHandler(int signum);

void guardarEnLog(ofstream& log, string numero, string huella, bool valida);

bool validarHuella(string huella, ifstream& ids);

void verificarProceso();

void trabajoProceso(Parametros parametros);


