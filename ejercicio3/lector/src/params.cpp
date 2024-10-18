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
        } else if (esParametroNumero(argv[i])) {
            if(!numeroProcesado) {
                if ((i + 1)<argc) {                   
                    validarEnteros(argv[i + 1]);
                    setNumero(stoi(argv[i + 1]));
                } else {
                    throw runtime_error("Debe ingresar un argumento para -n|--numero.");
                }
                numeroProcesado = true;
                i++; 
            } else {
                throw runtime_error("Ya se proceso el parametro -n|--numero.");
            } 
        } else if (esParametroMensajes(argv[i])) {
            if(!mensajesProcesado) {
                if ((i + 1)<argc) {                   
                    validarEnteros(argv[i + 1]);
                    setMensajes(stoi(argv[i + 1]));
                } else {
                    throw runtime_error("Debe ingresar un argumento para -m|--mensajes.");
                }
                mensajesProcesado = true;
                i++; 
            } else {
                throw runtime_error("Ya se proceso el parametro -m|--mensajes.");
            } 
        } else if (esParametroSegundos(argv[i])) {
            if(!segundosProcesado) {
                if ((i + 1)<argc) {                   
                    validarEnteros(argv[i + 1]);
                    setSegundos(stoi(argv[i + 1]));
                } else {
                    throw runtime_error("Debe ingresar un argumento para -s|--segundos.");
                }
                segundosProcesado = true;
                i++; 
            } else {
                throw runtime_error("Ya se proceso el parametro -s|--segundos.");
            } 
        } else if(seSolicitoAyuda(argv[i])) {
            throw runtime_error("No se puede solicitar ayuda si ingresa otros parametros.");
        } else {
            throw runtime_error("Parametros incorrectos.");
        }
    } 

    parametrosObligatorios();
}

void Parametros::setNumero(const int numero) {
    this->numero = numero;
}

void Parametros::setSegundos(const int segundos) {
    this->segundos = segundos;
}

void Parametros::setMensajes(const int mensajes) {
    this->mensajes = mensajes;
}

void Parametros::setIds(Archivo ids) { 
    this->ids = ids; 
}

int Parametros::getNumero() const {
    return numero;
}

int Parametros::getSegundos() const {
    return segundos;
}

int Parametros::getMensajes() const {
    return mensajes;
}

Archivo Parametros::getIds() const { 
    return ids; 
}

void Parametros::noSeIngresaronParametros(int argc) { 
    if(argc == 1) 
        throw runtime_error("No se ingresaron parametros.");
}

void Parametros::parametrosObligatorios() {
    if(!numeroProcesado || !mensajesProcesado || !segundosProcesado || !idsProcesado) 
        throw runtime_error("Los parametros -n|--numero, -m|--mensaje, -s|--segundo y -i|--ids son obligatorios.");
}

bool Parametros::esParametroNumero(const string arg) const {
    return !arg.compare("--numero") || !arg.compare("-n") ? true : false;
}

bool Parametros::esParametroSegundos(const string arg) const {
    return !arg.compare("--segundos") || !arg.compare("-s") ? true : false;
}

bool Parametros::esParametroMensajes(const string arg) const {
    return !arg.compare("--mensajes") || !arg.compare("-m") ? true : false;
}

bool Parametros::esParametroIds(const string arg) const {
    return !arg.compare("--ids") || !arg.compare("-i") ? true : false;
}

void Parametros::validarEnteros(const string &direc) {
    try {
        stoi(direc);

        // Si la conversión es exitosa, se puede continuar
    } catch (const std::invalid_argument &) {
        // Lanza una excepción si el string no puede ser convertido a entero
        throw runtime_error("La cadena no es un número válido.");
    } catch (const std::out_of_range &) {
        // Lanza una excepción si el número está fuera del rango de un entero
        throw runtime_error("El número está fuera del rango permitido.");
    }
}

void Parametros::validarIds(const string& direc) {
    filesystem::path ruta(direc);

    if (!filesystem::exists(ruta)) {
        throw std::runtime_error("El archivo Ids no existe.");
    }

    if (!filesystem::is_regular_file(ruta)) {
        throw std::runtime_error("La ruta de Ids no corresponde a un archivo.");
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
    cout << "-n / --numero      Numero del sensor. (Requerido)\n";
    cout << "-m / --mensajes    Cantidad de mensajes a enviar. (Requerido)\n";
    cout << "-s / --segundos    Intervalo de segundos para el envío del mensaje. (Requerido)\n";
    cout << "-i / --ids         Ruta del archivo de Ids validos. (Requerido)\n";
    cout << "Descripción: Este proceso validara las huellas de los sensores.\n\n";
}

void Parametros::mostrarParametros() const {
    cout << "Numero: " << getNumero()
        << "\nMensajes: " << getMensajes()
        << "\nSegundos: " << getSegundos()
        << "\nIds: " << getIds().getArchivo() << "\n" << endl;
}
