#include <funciones.hpp>

const char* FIFO_NAME = "/tmp/fifo_huellas";
char FIFO2_NAME[100];

ifstream archivoId;
int fifoFd;
int fifoFdLector;

void signalHandler(int signalNum) {

    if(archivoId.is_open()) {
        archivoId.close();
    }
    close(fifoFdLector);
    unlink(FIFO2_NAME);
    close(fifoFd);

    exit(signalNum);
}

void enviarHuellas(Parametros parametros) {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    filesystem::path archivoHuellas(FIFO_NAME);
    if(!exists(archivoHuellas)) {
        throw runtime_error("No esta en ejecucion el proceso central.");
    }

    string strFIFO2_NAME = "/tmp/fifo_lector" + to_string(parametros.getNumero());
    strcpy(FIFO2_NAME, strFIFO2_NAME.c_str());

    filesystem::path archivoSensor(FIFO2_NAME);
    if(exists(archivoSensor)) {
        throw runtime_error("Este sensor ya esta en ejecucion.");
    }
    
    ifstream archivoId(parametros.getIds().getArchivo());
    if (!archivoId.is_open()) {
        throw runtime_error("No se pudo abrir el archivo de IDs.");
    }

    fifoFd = open(FIFO_NAME, O_WRONLY);
    if (fifoFd == -1) {
        archivoId.close();
        throw runtime_error("No se pudo abrir el FIFO para escritura.");
    }

    mkfifo(FIFO2_NAME, 0666);
    char buffer[256];
    
    string id;

    for (int i = 0; i < parametros.getMensajes(); i++)
    {
        if(!getline(archivoId, id)) {
            archivoId.close();
            close(fifoFd);
            unlink(FIFO2_NAME);
            throw runtime_error("No hay mas IDs en el archivo de IDs.");
        }

        string mensaje = to_string(parametros.getNumero()) + " " + id;

        if(!exists(archivoHuellas)) {
            archivoId.close();
            close(fifoFd);
            unlink(FIFO2_NAME);
            throw runtime_error("Se detuvo el proceso central.");
        }
        write(fifoFd, mensaje.c_str(), mensaje.size() + 1);

        // El proceso se bloquea hasta que recibe si es valida o no la huella que envió
        if(!exists(archivoHuellas)) {
            archivoId.close();
            close(fifoFd);
            unlink(FIFO2_NAME);
            throw runtime_error("Se detuvo el proceso central.");
        }
        fifoFdLector = open(FIFO2_NAME, O_RDONLY);
        if (fifoFdLector == -1) {
            archivoId.close();
            close(fifoFd);
            unlink(FIFO2_NAME);
            throw runtime_error("No se pudo abrir el FIFO para lectura.");
        }
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = read(fifoFdLector, buffer, sizeof(buffer));
        if (bytesRead > 0) {
            cout << buffer << endl;
        }
        close(fifoFdLector);

        sleep(parametros.getSegundos()); // Esperar el intervalo
    }
    
    archivoId.close();
    close(fifoFd);

    unlink(FIFO2_NAME);
}