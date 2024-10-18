#include <funciones.hpp>

pid_t pidHijo1, pidHijo2, pidNieto1, pidNieto3, pidZombie, pidDemonio, pid1, pid2, pid3;
int pipefd[2];

void verificarProceso(pid_t pid, string nombre)
{
    try {
        if(pid < 0)
            throw runtime_error("No se pudo crear el proceso " + nombre +". Se omitira " + nombre +" en la jerarquia.");
    } catch(const std::exception& e) {
        cerr << "\033[31mError: " << e.what() << "\033[0m"  << endl;
    }
}

void trabajoProceso(string nombre)
{
    cout << "Soy el proceso " << nombre << " con PID " << getpid() << ", mi padre es " << getppid() << endl;
}

void simularTrabajo()
{
    // Simula trabajo indefinidamente a la espera de una señal
    while(true)
        sleep(1);
}

void crearZombie()
{
    trabajoProceso("Zombie");
    // Terminar el proceso hijo sin limpieza para que quede como proceso zombie (no es una buena practica, pero solo sera realizada en este ej a modo de demostración)
    _exit(0);
}

void crearDemonio()
{
    trabajoProceso("Demonio");

    // Crea una nueva sesión
    if (setsid() < 0) 
        throw runtime_error("No se pudo crear una nueva sesión para el proceso demonio.");

    simularTrabajo(); 
}

void finalizarProceso(pid_t pid)
{
    int status;

    if(pid>0) {
        kill(pid, SIGTERM); // Envía la señal SIGTERM al proceso
        waitpid(pid, &status, 0); //esperar a que finalice
    }

}

void presionarTecla()
{
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt); // Obtener la configuración actual del terminal
    newt = oldt;                     // Hacer una copia de la configuración
    newt.c_lflag &= ~(ICANON | ECHO); // Desactivar el modo canónico y el eco
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Aplicar la nueva configuración

    // Esperar a que se presione una tecla
    getchar();

    // Restaurar la configuración original del terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void crearJerarquia()
{
    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);

    // Se crea una tuberia para que sus Hijos se comuniquen con su Padre
     if (pipe(pipefd) == -1)
        throw runtime_error("No se pudo crear el pipe para la comunicación del Padre y sus Hijos.");

    // Padre
    trabajoProceso("Padre");

    pidHijo1 = fork();

    verificarProceso(pidHijo1, "Hijo1");

    if (pidHijo1 == 0) {
        // Hijo1
        trabajoProceso("Hijo1");

        pidNieto1 = fork();

        verificarProceso(pidNieto1, "Nieto1");
      
        if (pidNieto1 == 0) {
            // Nieto1
            trabajoProceso("Nieto1");
            simularTrabajo();
        } else {
            // Hijo1
            pidZombie = fork();

            verificarProceso(pidZombie, "Zombie");
            
            if(pidZombie == 0) {
                // Zombie
                crearZombie();
            } else {
                // Hijo1
                pidNieto3 = fork();

                verificarProceso(pidNieto3, "Nieto3");
                
                if (pidNieto3 == 0) {
                    // Nieto3
                    trabajoProceso("Nieto3");
                    simularTrabajo();
                } else {
                    // Hijo1

                    // Le trasmite a su Padre el PID de sus Hijos
                    close(pipefd[0]); 
                    write(pipefd[1], &pidNieto1, sizeof(pidNieto1)); 
                    write(pipefd[1], &pidNieto3, sizeof(pidNieto3)); 
                    close(pipefd[1]); 

                    simularTrabajo();
                }
            }
        }
    } else {
        // Padre
        pidHijo2 = fork();
    
        verificarProceso(pidHijo2, "Hijo2");

        if (pidHijo2 == 0) {
            // Hijo2
            trabajoProceso("Hijo2");

            pidDemonio = fork();

            verificarProceso(pidDemonio, "Demonio");

            if(pidDemonio == 0) {
                // Demonio
                try {
                    crearDemonio();
                } catch (const runtime_error& e) {
                    cerr << "\033[31mError: " << e.what() << ". Se manejara el demonio como un proceso normal. \033[0m" << endl;
                    simularTrabajo();
                }
                
            } else {
                // Hijo2

                // Le trasmite a su Padre el PID de su Hijo
                close(pipefd[0]); 
                write(pipefd[1], &pidDemonio, sizeof(pidDemonio)); 
                close(pipefd[1]); 

                // Espera a su hijo
                simularTrabajo();
            }
        } else {
            // Padre
            sleep(1); // Tiempo suficiente para que los procesos hagan su trabajo

            cout << "\nPara verificar la jerarquía de procesos, abre otra consola y usa el comando 'ps' o 'pstree'." << endl;
            cout << "Presione cualquier tecla para finalizar los procesos..." << endl;
            presionarTecla(); // Se pausa el proceso hasta que se presiona una tecla

            cout << "\nFinalizando procesos..." << endl;
            finalizarJerarquia();

            sleep(1);
            cout << "Todos los procesos han sido finalizados correctamente." << endl;
            sleep(1);

            cout << "\nPuedes verificar nuevamente cómo se ha finalizado la jerarquía de procesos con el comando 'ps' o 'pstree'. Solo quedará el proceso Padre." << endl;
            cout << "Presiona cualquier tecla para finalizar el proceso Padre..." << endl;
            presionarTecla(); // Se pausa el proceso hasta que se presiona una tecla
            cout << "\nFinalizando proceso Padre..." << endl;
    
        }
    }
}

void finalizarJerarquia() {
    close(pipefd[1]); 
    if(pidHijo2>0) {
        read(pipefd[0], &pid1, sizeof(pid1));
        finalizarProceso(pid1);
    }
    if(pidHijo1>0) {
        read(pipefd[0], &pid2, sizeof(pid2)); 
        read(pipefd[0], &pid3, sizeof(pid3)); 
        finalizarProceso(pid2);
        finalizarProceso(pid3);  
    }
    close(pipefd[0]); 
    finalizarProceso(pidHijo1);
    finalizarProceso(pidHijo2);
}

void signalHandler(int signalNum) {

    if(signalNum==2)
    	cout << "\nSeñal SIGINT recibida." << endl;

    if(signalNum==15)
    	cout << "\nSeñal SIGTERM recibida." << endl;

    cout << "Finalizando jerarquia..." << endl;
    finalizarJerarquia();

    if(signalNum==15 || signalNum==2)
    	exit(0);

    exit(signalNum);
}