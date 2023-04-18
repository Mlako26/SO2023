#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define NOMBRE_ARCHIVO = "algo.txt";
#define N = 3;
#define M = 3;

int cargar_fila(const int fd, int* lista);
int cargar_fila(const int fd, int* lista);
void sort(char *s);
int dup2(int oldfd, int newfd);

int main(int argc, char **argv)
{
    
    int pipes_go[7][2];
    int pipes_return[7][2];

    for (int i = 0; i < 8; i++) {
        if (pipe(pipes_go[i]) == -1) {
            exit(-1);
        }
        if (pipe(pipes_return[i]) == -1) {
            exit(-1);
        }
        close(pipes_go[i][0]);
        close(pipes_return[i][1]);
    }

    int proc_amount = 0; // Cantidad de procesos calculando
    int remaining_rows = N;
    int i = 0;

    while (remaining_rows > 0) {
        if (i == 7) i = 0;
        if (proc_amount < 8) {
            pid_t pid = fork();
            if (pid == 0) {
                child(pipes_go, pipes_return, i);
            }
        }
        int* lista; 
        if (cargar_fila(NOMBRE_ARCHIVO, lista) == -1) {
            exit(-1);
        }
        write(pipes_go[i][1], &lista, M);
        
    }
	return 0;
}
