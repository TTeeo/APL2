#include <params.hpp>


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
        if (esParametroIds(argv[i])) {
            if(!idsProcesado) {
                if ((i + 1)<argc) {
                    validarIds(argv[i + 1]);
                } else {
                    throw runtime_error("Debe ingresar un argumento para -i|--ids");
                }
            } else {
                throw runtime_error("Ya se proceso el parametro -i|--ids.");
            }
            idsProcesado = true;
            i++;
        } else if (esParametroLog(argv[i])) {
            if(!logProcesado) {
                if ((i + 1)<argc) {                   
                    validarLog(argv[i + 1]);
                } else {
                    throw runtime_error("Debe ingresar un argumento para -l|--log.");
                }
                logProcesado = true;
                i++; 
            } else {
                throw runtime_error("Ya se proceso el parametro -l|--log.");
            }

        } else if(seSolicitoAyuda(argv[i])) {
            throw runtime_error("No se puede solicitar ayuda si ingresa otros parametros.");
        } else {
            throw runtime_error("Parametros incorrectos.");
        }
    } 

    parametrosObligatorios();
}

void Parametros::setLog(Archivo log) { 
    this->log = log; 
}

void Parametros::setIds(Archivo ids) { 
    this->ids = ids; 
}

Archivo Parametros::getLog() const { 
    return log; 
}

Archivo Parametros::getIds() const { 
    return ids; 
}

void Parametros::noSeIngresaronParametros(int argc) { 
    if(argc == 1) 
        throw runtime_error("No se ingresaron parametros.");
}

void Parametros::parametrosObligatorios() {
    if(!logProcesado || !idsProcesado) 
        throw runtime_error("Los parametros -l|--log y -i|--ids son obligatorios.");
}

bool Parametros::esParametroLog(const string arg) const {
    return !arg.compare("--log") || !arg.compare("-l") ? true : false;
}

bool Parametros::esParametroIds(const string arg) const {
    return !arg.compare("--ids") || !arg.compare("-i") ? true : false;
}

void Parametros::validarLog(const string &direc) {
    filesystem::path ruta(direc);

    if (ruta.extension() != ".log") {
        throw runtime_error("El archivo de Log no tiene la extensión .log.");
    }

    filesystem::path rutaPath = ruta.parent_path();

    if (!filesystem::exists(rutaPath)) {
        throw runtime_error("El directorio del Log no existe.");
    }

    if (filesystem::exists(ruta) && filesystem::is_regular_file(ruta)) {
        Archivo nuevo(direc);
        setLog(nuevo);
    } else if (filesystem::is_directory(rutaPath)) {
        filesystem::path archivoNuevo = direc;  
        ofstream nuevoArchivo(archivoNuevo);
        if (!nuevoArchivo) {
            throw runtime_error("No se pudo crear el archivo Log en el directorio.");
        }
        Archivo nuevo(archivoNuevo);
        setLog(nuevo);
    } else {
        throw runtime_error("La ruta de Log no corresponde a un directorio, ni a un archivo.");
    }
}

void Parametros::validarIds(const string& direc) {
    filesystem::path ruta(direc);

    if (!filesystem::exists(ruta)) {
        throw runtime_error("El archivo Ids no existe.");
    }

    if (!filesystem::is_regular_file(ruta)) {
        throw runtime_error("La ruta de Ids no corresponde a un archivo.");
    }

    Archivo nuevo(direc);
    setIds(nuevo);
}

bool Parametros::seSolicitoAyuda(const string arg) const {
    return !arg.compare("--help") || !arg.compare("-h") ? true : false;
}

void Parametros::mostrarAyuda() const {

    cout << "\033[34mAyuda\033[0m\n"; // Título en azul
    cout << "Parámetros:\n";
    cout << "-l / --lod     Ruta del archivo Log. (Requerido)\n";
    cout << "-i / --ids     Ruta del archivo de Ids validos. (Requerido)\n";
    cout << "Descripción: Este proceso validara las huellas de los sensores.\n\n";
}

void Parametros::mostrarParametros() const {
    cout << "Log: " << getLog().getArchivo()
        << "\nIds: " << getIds().getArchivo() << "\n" << endl;
}
