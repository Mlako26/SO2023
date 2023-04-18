#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

void bart() {
    write(1, "Soy Bart\n", 9);
	sleep(1);
}

void lisa() {
    write(1, "Soy Lisa\n", 9);
	sleep(1);
}

void maggie() {
    write(1, "Soy Maggie\n", 11);
	sleep(1);
}

void homer() {    
    write(1, "Soy Homer\n", 10);
	sleep(1); 
    pid_t pid = fork();

    if (pid == 0) {
        bart();
    } else {
        wait(NULL);
        pid = fork();
        if (pid == 0) {
            lisa();
        } else {
            wait(NULL);  
            pid = fork();
            if (pid == 0) {
                maggie();
            } else {
                wait(NULL);
            }
        }
    }
}

void abraham() {
    wait(NULL);
    write(1, "Soy Abraham\n", 12);
	sleep(1);
}

int main(int argc, char const *argv[]){
  pid_t pid = fork();
  //si no hay error, pid vale 0 para el hijo
  //y el valor del process id del hijo para el padre
  if (pid == -1) exit(EXIT_FAILURE);  
  //si es -1, hubo un error
  if (pid == 0) {
	homer();
  } else {
	abraham();
  }
  exit(EXIT_SUCCESS); //cada uno finaliza su proceso
}
