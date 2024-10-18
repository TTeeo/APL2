#include "params.hpp"

Parametros::Parametros(int argc, char *argv[]) {

    noSeIngresaronParametros(argc);

    if(argc==2) {
        if (seSolicitoAyuda(argv[1])) {
            mostrarAyuda();
            exit(EXIT_SUCCESS);
        } else {
            throw runtime_error("Parametros invalidos.");
        }
    }

    for (int i = 1; i < argc; i++) {    
        if (esParametrNroThreads(argv[i])) {
            if(!nroThreadsProcesado) {
                if ((i + 1)<argc) {
                    validarNroThread(argv[i + 1]);
                    nroThreads = atoi(argv[i + 1]);
                } else {
                    throw runtime_error("Debe ingresar un argumento para -t|--threads.");
                }
                nroThreadsProcesado = true;
                i++; 
            } else {
                throw runtime_error("Ya se proceso el parametro -t|--threads.");
            }

        } else if (esParametroDirectorio(argv[i])) {
            if(!directorioProcesado) {
                if ((i + 1)<argc) {
                    validarDirectorio(argv[i + 1]);
                    Directorio directorioNuevo(argv[i + 1]);
                    directorio = directorioNuevo;
                } else {
                    throw runtime_error("Debe ingresar un argumento para -d|--directorio");
                }
            } else {
                throw runtime_error("Ya se proceso el parametro -d|--directorio.");
            }
            directorioProcesado = true;
            i++; 

        } else if(esParametrCadena(argv[i])) {
            if(!cadenaProcesado) {
                if((i + 1)<argc) {
                    Cadena cadenaNueva(argv[i+1]);
                    cadena = cadenaNueva;
                } else {
                    throw runtime_error("Debe ingresar un argumento para -c|--cadena.");
                }
            } else {
                throw runtime_error("Ya se proceso el parametro -c|--cadena.");
            }
            cadenaProcesado = true;
            i++;

        } else if(seSolicitoAyuda(argv[i])) {
            throw runtime_error("No se puede solicitar ayuda si ingresa otros parametros.");
        } else {
            throw runtime_error("Parametros invalidos.");
        }
    } 

    parametrosObligatorios();
}

void Parametros::setDirectorio(Directorio directorio) { 
    this->directorio = directorio; 
}

void Parametros::setNroThreads(int nroThreads) { 
    this->nroThreads = nroThreads; 
}

void Parametros::setCadena(Cadena cadena) { 
    this->cadena = cadena; 
}

Directorio Parametros::getDirectorio() const { 
    return directorio; 
}

int Parametros::getNroThreads() const { 
    return nroThreads; 
}

Cadena Parametros::getCadena() const { 
    return cadena; 
}

void Parametros::noSeIngresaronParametros(int argc) { 
    if(argc == 1) 
        throw runtime_error("No se ingresaron parametros.");
}

void Parametros::parametrosObligatorios() {
    if(!directorioProcesado || !nroThreadsProcesado || !cadenaProcesado) 
        throw runtime_error("Los parametros -d|--directorio, -t|--threads y -c|--cadena son obligatorios.");
}

bool Parametros::esParametroDirectorio(const string arg) const {
    return !arg.compare("--directorio") || !arg.compare("-d") ? true : false;
}

bool Parametros::esParametrNroThreads(const string arg) const {
    return !arg.compare("--threads") || !arg.compare("-t") ? true : false;
}

bool Parametros::esParametrCadena(const string arg) const {
    return !arg.compare("--cadena") || !arg.compare("-c") ? true : false;
}

void Parametros::validarDirectorio(const string &direc) const {
    filesystem::path ruta(direc);

    if (!filesystem::exists(ruta)) {
        throw std::runtime_error("El directorio no existe.");
    }

    if (!filesystem::is_directory(ruta)) {
        throw std::runtime_error("La ruta no corresponde a un directorio.");
    }
}

void Parametros::validarNroThread(const string& str) const {
    try {
        size_t idx;
        int numero = stoi(str, &idx);

        // Si no es un número válido o no es positivo, lanzamos la excepción
        if (!(idx == str.length() && numero > 0)) {
            throw runtime_error("El Nro de Thread no es valido. Debe ser un entero positivo.");
        }

    } catch (const invalid_argument& e) {
        throw runtime_error("El Nro de Thread no es valido. Debe ser un entero positivo.");
    } catch (const out_of_range& e) {
        throw runtime_error("El Nro de Thread no es valido. Fuera de rango.");
    }
}

bool Parametros::seSolicitoAyuda(const string arg) const {
    return !arg.compare("--help") || !arg.compare("-h") ? true : false;
}

void Parametros::mostrarAyuda() const {

    cout << "\033[34mAyuda\033[0m\n"; // Título en azul
    cout << "Parámetros:\n";
    cout << "-t / --threads        Cantidad de threads a ejecutar concurrentemente para procesar los archivos del directorio (Requerido). El número ingresado debe ser un entero positivo.\n";
    cout << "-d / --directorio     Ruta del directorio a analizar. (Requerido)\n";
    cout << "-c / --cadena         Cadena a buscar. (Requerido)\n";
    cout << "-h / --help           Muestra la ayuda del ejercicio.\n";
    cout << "Descripción: Los Threads buscaran los archivos del directorio que contengan la cadena.\n\n";
}

void Parametros::mostrarParametros() const {
    cout << "Directorio: " << getDirectorio().getDirectorio()
        << "\nCadena: " << getCadena().getCadena()
        << "\nNro de Threads: " << getNroThreads() << "\n" << endl;
}


