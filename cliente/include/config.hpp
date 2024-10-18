#pragma once

#define TAM_PREGUNTA 128
#define TAM_OPCIONES 128
#define CANT_OPCIONES 3
#define TAM_MSJ_SERVIDOR 128
#define TAM_NICKNAME 64
#define TAM_BUFFER 1024

typedef struct {

  char pregunta[TAM_PREGUNTA];
  char opciones[CANT_OPCIONES][TAM_OPCIONES];
  char mensaje[TAM_MSJ_SERVIDOR];
  bool partidaEnCurso;

} MensajeServidor;

typedef struct {

  char nickname[TAM_NICKNAME];
  int opcionElegida;

} MensajeCliente;

typedef struct {

  char nickname[TAM_NICKNAME];
  int puntaje;
  int posicion;

} Resultado;