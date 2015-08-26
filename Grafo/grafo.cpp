#include "grafo.h"
#include <cstring>
#include <iostream>
#include <cstdio>

using namespace std;

Grafo::Grafo(){
	raiz = 0;
}

Grafo::Grafo(Nodo * nodo){
	raiz = nodo;
}

int Grafo::agregar(Nodo * nodo, char * id){
	int exito = 0;
	if(!raiz){
		raiz = nodo;
		exito = 1;
		//cout << "agregada raiz: "<<id<<endl;
	}
	else{ //buscar nodo con id
		exito = agregarR(raiz, id, nodo); 
	}
	return exito;
}

int Grafo::agregarR(Nodo * padre, char * id, Nodo * nodo){
	int sucs = 0;
	int encontrado = 0;
	if(strcmp(padre->id_nodo(),id) == 0){
		//cout << "agregando nodo: "<<nodo->id_nodo() << " a nodo "<<id<<endl;
		padre->agregarSucesor(nodo);
		encontrado = 1;		
	}
	else{
		sucs = padre->cantSucesores();
		//cout << "expandiendo "<<sucs<<" sucesores en nodo "<<padre->id_nodo()<<endl;
		for(int i=0;i<sucs && !encontrado;i++){
			encontrado = agregarR(padre->getSucesor(i), id, nodo);
		}
	}
	return encontrado;
}

void Grafo::verContenido(){
	if(!raiz)
		cout << "Grafo vacio\n";
	else
		verContenidoR(raiz);
}

void Grafo::verContenidoR(Nodo * nodo){
	cout << "Nodo tipo '" << nodo->isa_txt() << "', ID = "<<nodo->id_nodo()<<endl;
	int sucs = 0;
	if(nodo->isa() != CASO){
		sucs = nodo->cantSucesores();
		for(int i=0;i<sucs;i++){
			verContenidoR(nodo->getSucesor(i));
		}
	}
}

/*
 * CONSIDERAR LA POSIBILIDAD DE GENERAR LISTAS A PARTIR DE CUALQUIER NODO, LO CUAL ES TRIVIAL (CAMBIAR EL NODO INICIAL)
 * EL PROBLEMA ES COMO HACER LA REFERENCIA DESDE AFUERA
 * 
 * */
char * Grafo::generarLista(){
	char * lista;
	
	if(raiz){
		lista = new char[4096];
    	this->listaR(raiz, lista);
	}
 	else
    	lista = (char *) "()";
	
	return lista;
}

void Grafo::listaR(Nodo * nodo, char * lista){
	int cant_sucs = nodo->cantSucesores();

    //si es caso
    if(nodo->isa() == CASO){
      //cout << "("<<nodo->isa_txt() << nodo->verParesAV() << ")";
      sprintf(lista, "%s(%c[%s]%s)", lista, nodo->isa_txt(),nodo->id_nodo(),nodo->verParesAV());
    }
    else{      
      //cout << "("<<nodo->isa_txt() << nodo->verParesAV() << "(";
      sprintf(lista, "%s(%c[%s]%s (",lista, nodo->isa_txt(),nodo->id_nodo(),nodo->verParesAV());
      for(int i=0; i<cant_sucs; i++){
		this->listaR(nodo->getSucesor(i), lista);
      }
      //cout << "))";
      sprintf(lista, "%s))", lista);
    }
}
