#include "indice.h"

Indice::Indice(char * etiqueta){
  this->tipo = INDICE;
//  this->id = new char[strlen(id)];
  strcpy(this->id,etiqueta);
  this->sucesores = new ListaNodos();
  this->lista = new ListaAV();
  this->tipo_txt = 'I';
}

char * Indice::getAtributo(){
	char * r = (char *) "";
	if(lista->longitud() > 0) r = this->lista->getParAV(0)[0];
	
	return r;
}

char * Indice::getValor(int pos){
	char * r = (char *) "";
	if(lista->longitud() > 0 && pos < lista->longitud()) r = this->lista->getParAV(pos)[1];
	
	return r;
}

Nodo * Indice::getSucesor(char * valor){
	int lon = this->lista->longitud();
	int encontrado = 0;
	Nodo * n = 0;
	for(int i=0;i<lon && !encontrado; i++){
		if( strcmp(this->getValor(i), valor) == 0 ){
			encontrado = 1;
			n = (Nodo *) this->sucesores->getSucesor(i);
		}
	}

	return n;
}
