#include <params.hpp>

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <csignal>
#include <ctime>
#include <cstring>
#include <sys/stat.h>
#include <filesystem>

void enviarHuellas(Parametros parametros);

void signalHandler(int signalNum);