#pragma once

#include <string>

using namespace std;

const string PATH_SHMFILE = "/tmp/shmfile";
const int PROJECT_ID = 66;
const string NOMBRE_SEM_CLIENTE = "/semaforo_cliente";
const string NOMBRE_SEM_SERVIDOR = "/semaforo_servidor";

#define TAM_PREGUNTA 512
#define TAM_OPCION 256
#define TAM_MENSAJE_SERVIDOR 512
#define CANT_OPCIONES 3

typedef struct {
  char pregunta[TAM_PREGUNTA];
  char opciones[CANT_OPCIONES][TAM_OPCION];
  char mensajeServidor[TAM_MENSAJE_SERVIDOR];
  int respuestaCliente;
  int puntajeFinal;
  bool partidaEnCurso;

} DatosCompartidos;