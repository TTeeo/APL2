#include <Hilo.hpp>

mutex mtxPos; // Mutex para proteger el acceso al siguiente archivo
mutex mtxPant; // Mutex para proteger el acceso a la salida por pantalla
Directorio directorio;

atomic<bool> stopSignal(false);

void signalHandler(int signalNum) {
    if(signalNum==2)
    	cout << "\nSeñal SIGINT recibida." << endl;

    if(signalNum==15)
    	cout << "\nSeñal SIGTERM recibida." << endl;

    cout << "Finalizando programa..." << endl;

    stopSignal = true;
}


void crearHilos(Parametros parametros) 
{
    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);

    vector<thread> hilos; // Vector para almacenar los hilos

    directorio = parametros.getDirectorio();
    directorio.cargarArchivos();

    // Crear n hilos
    for (int i = 1; i <= parametros.getNroThreads(); ++i) {
        hilos.emplace_back(funcionHilo, i, parametros.getCadena()); // Agregar cada hilo al vector
    }

    // Esperar a que todos los hilos terminen
    for (auto& hilo : hilos) {
        hilo.join(); // Espera a que el hilo termine
    }

}

void funcionHilo(int id, Cadena cadena) 
{
    Archivo archivo("");

    // Leer palabras hasta que no queden palabras
    while (!stopSignal) {
        mtxPos.lock(); // Bloquear el mutex para acceder directorio

        if (!directorio.obtenerArchivo(archivo)) {
            mtxPos.unlock();
            break;
        }
        
        mtxPos.unlock(); // Desbloquear el mutex

        try {
            buscarPalabra(archivo, id, cadena);
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << '\n';
        }
    }
}

void buscarPalabra(Archivo archivo, int id, Cadena cadena)
{
    string linea;
    int nroLinea=1;
    Cadena contenido("");

    ifstream archivoAbierto(directorio.getDirectorio() + "/" + archivo.getNombre());
    if (archivoAbierto.is_open()) {
        while (getline(archivoAbierto, linea)) {

            if (stopSignal) {
                break;  // Si se recibe la señal, salir del bucle
            }

            contenido.setCadena(linea);
            
            if(cadena.estaContenidaEn(contenido)) {
                mtxPant.lock(); // Bloquear el mutex para imprimir en pantalla
                cout << "Nro de Tread: " << id << " | Archivo: " << archivo.getNombre() << " | Linea: " << nroLinea << endl;
                mtxPant.unlock(); // Desbloquear el mutex
            } 
            nroLinea++;  // Incrementar el contador de líneas
        }
        archivoAbierto.close();  // Cerrar el archivo al finalizar
    } else {
        throw runtime_error("No se pudo abrir el archivo "+archivo.getNombre()+", se omitira su lectura.");
    }
}