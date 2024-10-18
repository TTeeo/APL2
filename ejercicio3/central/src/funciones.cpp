#include <funciones.hpp>

const char* PID_NAME = "/tmp/pidProcesoCentral.txt";

const char* FIFO_NAME = "/tmp/fifo_huellas";

const char* FIFO2_NAME = "/tmp/fifo_lector";

int fifoFd;
int fifoFdLector;

ifstream archivoId;  
ofstream archivoLog; 

void crearDemonio() {

    pid_t pid = fork();

     if (pid < 0) {
        throw runtime_error("No se pudo crear el proceso");
    }

    // Terminar el proceso padre
    if (pid > 0) {
        
        filesystem::path rutaArchivoPid(PID_NAME);
        ofstream archivoPid(rutaArchivoPid);
        if (!archivoPid.is_open()) {
            throw runtime_error("No se pudo abrir el archivo para guardar el PID.");
        }
        archivoPid << pid;
        archivoPid.close();
        

        cout << "Proceso central iniciado, se ejecutara como proceso demonio. Para finalizarlo ingrese 'kill " 
            << pid << "'\n" << endl;
        exit(0);
    }

    // Crear una nueva sesión
    if (setsid() < 0) {
        throw runtime_error("No se pudo crear una nueva sesión para el proceso demonio.");
    }

    // Cambiar el directorio de trabajo
    chdir("/");

}

void signalHandler(int signum) {
    
    // Cerrar el descriptor del FIFO
    close(fifoFd);

    // Cerrar los archivos abiertos
    if (archivoId.is_open()) {
        archivoId.close();
    }

    if (archivoLog.is_open()) {
        archivoLog.close();
    }

    // Eliminar el archivo FIFO y PID
    unlink(FIFO_NAME);
    unlink(PID_NAME);

    exit(signum);
}

void guardarEnLog(ofstream& log, string numero, string huella, bool valida) {

    time_t ahora = time(nullptr);
    
    // Convertir el tiempo a formato local
    tm* tiempoLocal = localtime(&ahora);
    
    // Escribir la fecha y hora formateadas en el archivo de log
    if(valida)
        log << put_time(tiempoLocal, "%Y-%m-%d %H:%M:%S") << " Sensor: " << numero << " Huella valida: " << huella << endl;
    else
        log << put_time(tiempoLocal, "%Y-%m-%d %H:%M:%S") << " Sensor: " << numero << " Huella invalida: " << huella << endl;
    
}

bool validarHuella(string huella, ifstream& ids) {
    string linea;

    while (getline(ids, linea)) {
        // Comparar la línea con la palabra
        if (linea == huella) {
            return true;  // Salir del bucle si encuentras la palabra
        }
    }

    return false;
}

void verificarProceso() {
    filesystem::path rutaArchivoPid(PID_NAME);

    if(filesystem::exists(rutaArchivoPid)) {

        ifstream archivoPid(rutaArchivoPid);

        if (!archivoPid.is_open()) {
            throw runtime_error("Ya hay un proceso central en ejecución, pero no se pudo abrir el archivo con el PID.");
        }

        string pid;
        archivoPid >> pid;  // Lee el PID del archivo
        archivoPid.close();

        throw runtime_error("Ya hay un proceso central en ejecución. Para finalizarlo ingresa 'kill "+ pid + "'. ");
    }
}

void trabajoProceso(Parametros parametros) {
    
    verificarProceso();
    
    archivoId.open(parametros.getIds().getArchivo());
    if (!archivoId.is_open()) {
        throw runtime_error("No se pudo abrir el archivo de IDs");
    }

    archivoLog.open(parametros.getLog().getArchivo());
    if (!archivoLog.is_open()) {
        throw runtime_error("No se pudo abrir el archivo de Log");
    }

    crearDemonio();

    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);

    mkfifo(FIFO_NAME, 0666);
    
    char buffer[256];
    int fifoFd = open(FIFO_NAME, O_RDONLY);

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = read(fifoFd, buffer, sizeof(buffer));

        // Si se leen datos
        if (bytesRead > 0) {
            // Procesar el buffer como una cadena de caracteres
            char* ptr = buffer;
            while (ptr < buffer + bytesRead) {
                // Leer cada mensaje hasta el carácter nulo
                string mensaje(ptr);
                size_t pos = mensaje.find(' ');
                string numero = mensaje.substr(0, pos);
                string id = mensaje.substr(pos + 1);
                bool valida = validarHuella(id, archivoId);

                guardarEnLog(archivoLog, numero, id, valida);

                string esValida;
                if(valida)
                    esValida = "Huella valida: " + id;
                else
                    esValida = "Huella invalida: " + id;

                char nuevoFIFO2_NAME[100];
                strcpy(nuevoFIFO2_NAME, FIFO2_NAME);
                strcat(nuevoFIFO2_NAME, numero.c_str());

                int fifoFdLector = open(nuevoFIFO2_NAME, O_WRONLY);
                write(fifoFdLector, esValida.c_str(), esValida.size() + 1);
                close(fifoFdLector);

                // Volver al principio del archivo
                archivoId.clear();  // Limpiar los flags de estado del archivo (EOF)
                archivoId.seekg(0, ios::beg);  // Mover el puntero de lectura al byte 0

                // Mover el puntero al siguiente mensaje
                ptr += strlen(ptr) + 1; // Sumar el tamaño del mensaje y el carácter nulo
            }
        } 

        // Si no se leen datos espero a que haya un nuevo proceso lector
        if(bytesRead == 0) {
            close(fifoFd);
            fifoFd = open(FIFO_NAME, O_RDONLY);
        }
    }
}



