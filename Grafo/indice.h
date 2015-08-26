#ifndef _indice_
#define _indice_
#include "nodo.h"

class Indice : public Nodo{
	
  	public:
    	Indice(char *);
    	//Indice(string);
    	char * getAtributo();
    	char * getValor(int);
    	Nodo * getSucesor(char * valor); //Obtener el sucesor N a partir del valor que lo indiza.

};

#endif
