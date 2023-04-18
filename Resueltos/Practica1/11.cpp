#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

// No se entienden muy bien las consignas, lo dejo aca.
// Si lo tengo que hacer con linux usaria getpid y getppid pero la verdad no se como conseguir la id del padre asi no mas sin esas calls (no se si esta bien asumir que el hijo tiene siempre id consecutivo)

int main(int argc, char const *argv[]){
    int current = 0;
    pid_t pid = fork();
    //si no hay error, pid vale 0 para el hijo
    //y el valor del process id del hijo para el padre
    if (pid == -1) exit(EXIT_FAILURE);  
    //si es -1, hubo un error
    if (pid == 0) {
        pid = get_current_pid() + 1;
        while(1) {
            current = breceive(pid);
            current++;
            bsend(dad, current);
        }
    } 

    pid_t pid2 = fork();

    if (pid2 == -1) exit(EXIT_FAILURE);  
    //si es -1, hubo un error
    if (pid2 == 0) {
        pid2 = get_current_pid() - 2;
        while(1) {
            current = breceive(dad);
            current++;
            bsend(dad, current);
        }
    } 

    while(1) {
        bsend(pid, current);
        current = breceive(pid);
        current++;
    }

    exit(EXIT_SUCCESS); //cada uno finaliza su proceso
}
