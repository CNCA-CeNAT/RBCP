#include "norma.h"

Norma::Norma(char * etiqueta){
  this->tipo = NORMA;
  //this->id = new char[strlen(id)];
  strcpy(this->id,etiqueta);
  this->lista = new ListaAV();
  this->sucesores = new ListaNodos();
  this->tipo_txt = 'N';
  this->gen = 0;
}

ListaAV * Norma::getListaAV(){
	return this->lista;
}

int Norma::generalizada(){
	return gen;
}

void Norma::marcarGeneralizada(){
	this->gen = 1;
}
