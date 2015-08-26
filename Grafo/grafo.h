#ifndef GRAFO_H_
#define GRAFO_H_
#include "caso.h"
#include "norma.h"
#include "indice.h"

/*
 * Grafo dirigido aciclico, para organizar los nodos tipo caso, norma e indice.
 * Cada nodo posee una lista de sucesores (ListaNodos), con un puntero a nodo, atributo, y valor. 
 * */

class Grafo{
	private:
		Nodo * raiz;
		int agregarR(Nodo *, char *, Nodo *);
		void verContenidoR(Nodo *);
		void listaR(Nodo *, char *);
		
	public:
		Grafo();
		Grafo(Nodo *);
		//Agregar un nodo
		int agregar(Nodo *, char * id);
		//Generar la representacion en lista de este grafo
		char * generarLista();
		//Desplegar en pantalla el grafo
		void verContenido();
};

#endif /*GRAFO_H_*/
