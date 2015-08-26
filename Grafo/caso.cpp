#include "caso.h"
#include <iostream>

using namespace std;

Caso::Caso(char * etiqueta){
  this->tipo = CASO;
//  this->id = new char[strlen(id)];
  strcpy(this->id,etiqueta);
  this->lista = new ListaAV();
  //cout << "listaAV creada"<<endl;
  this->sucesores = new ListaNodos();
  //cout << "listanodos creada"<<endl;
  this->tipo_txt = 'C';
  //cout << "creado CASO con id: " << id <<endl;
}

void Caso::setAV(char * atr, char * val){
	this->lista->agregar(atr,val,CASO);
}

void Caso::agregarListaAV(char ** atributos, char ** valores, int cantidad){
	for(int i=0;i<cantidad;i++){
		this->lista->agregar(atributos[i], valores[i], CASO);
	}
}
