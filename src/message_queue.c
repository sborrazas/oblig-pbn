#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "utils/debug.h"

int main(int argc, const char* argv[]) {
    return 0;
    /*
    int a = 1;
    int pid = fork();
    printf("hola!\n");

    if (pid == 0) {
      printf("soy el hijo y tengo pid %d!\n", getpid());
      exec("build/hijo");
      init_cm(mem);
      a++;
    }
    else if (pid > 0) {
      printf("soy el padre y tengo pid %d y mi hijo tiene pid %d!\n", getpid(), pid);
      a--;
    }

    printf("A TIENE VALOR: %d\n", a);*/
}
/*
int callback() { // Se llama cuando un hijo se muere (SIGCHLD)
  int pid_hijo = wait();

  // Sacar hijo de mi lista de hijos
}
*/
