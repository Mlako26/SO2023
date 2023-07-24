#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

void child(char const *command){
    exec(command);
}

void dad(){
    wait(NULL);
}

int main(int argc, char const *argv[]){
    if (argc != 2) {
        exit(EXIT_FAILURE); 
    }
    pid_t pid = fork();
    //si no hay error, pid vale 0 para el hijo
    //y el valor del process id del hijo para el padre
    if (pid == -1) exit(EXIT_FAILURE);  
    //si es -1, hubo un error
    if (pid == 0) {
        child(argv[1]);
    } else {
        dad();
    }
    exit(EXIT_SUCCESS); //cada uno finaliza su proceso
}
