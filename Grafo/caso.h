#ifndef _caso_
#define _caso_
#include "nodo.h"

class Caso : public Nodo{

  public:
    Caso(char *);
    void setAV(char *, char*);  //agregar solo el primer par, ya no se usa
    void agregarListaAV(char **, char **, int); // Agregar N pares de un caso en la lista de paresAV

};

#endif
