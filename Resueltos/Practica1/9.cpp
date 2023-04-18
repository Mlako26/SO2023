#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

void jorge(){
    write(1, "Soy Jorge\n", 10);
    sleep(1);
}

void jeniffer(){
    write(1, "Soy Jennifer\n\0", 14);
    sleep(1);
}

void julieta(){
    write(1, "Soy Julieta\n\0", 12);
    sleep(1);
    pid_t pid = fork();
    if (pid == 0) {
        jeniffer();
    } else {}
}

void juan(){
    write(1, "Soy Juan\n\0", 10);
    sleep(1);
    wait(NULL);

    pid_t pid = fork();

    if (pid == 0) {
        jorge();
    } else {}
}

int main(int argc, char const *argv[]){

    pid_t pid = fork();
    //si no hay error, pid vale 0 para el hijo
    //y el valor del process id del hijo para el padre
    if (pid == -1) exit(EXIT_FAILURE);  
    //si es -1, hubo un error
    if (pid == 0) {
        julieta();
    } else {
        juan();
    }
    exit(EXIT_SUCCESS); //cada uno finaliza su proceso
}
