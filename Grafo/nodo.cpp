#include "nodo.h"
#include <iostream>

//Borrado recursivo
//Si se borra la raiz de la red/grafo, se deberian borrar todos los nodos recursivamente
Nodo::~Nodo(){
	//int s = this->sucesores->longitud();
	//for(int i=0;i<s;i++) delete this->getSucesor(i);
}

char * Nodo::id_nodo(){
	return id;
};

short Nodo::isa(){
	return tipo;
};

char Nodo::isa_txt(){
    /*char texto;

	switch(tipo){ 
		case NORMA: 
			//strcpy(texto,"norma");
			texto] = 'N';
			texto[1] = '\0';
			break;
		case INDICE:
			//strcpy(texto, "indice");
			texto[0] = 'I';
			texto[1] = '\0';
			break;
		case CASO:
			//strcpy(texto,"caso");
			texto[0] = 'C';
			texto[1] = '\0';
			break;
		default:
			cout << "no es ninguno!!!"<<endl;
			break;
  	}*/

	return tipo_txt;
};

/*
 * Sucesores
 * */
void Nodo::agregarSucesor(Nodo * nodo){
	//cout << "Agregando sucesor con ID: "<<nodo->id_nodo()<<endl;
	this->sucesores->agregar( (void *) nodo, nodo->isa(), nodo->id_nodo());
};

Nodo * Nodo::getSucesor(int pos){
	return (Nodo *) this->sucesores->getSucesor(pos);
};

int Nodo::cantSucesores(){
	return this->sucesores->longitud();
}

/*
 * Pares (a,v)
 * */

void Nodo::agregarAV(char * a, char * v){
	lista->agregar(a,v,tipo);
}

int Nodo::cantParesAV(){
	return lista->longitud(); 
}

char ** Nodo::getParAV(int pos){
	return lista->getParAV(pos);
}

char * Nodo::verParesAV(){
	char * paresav;

	if(this->lista->longitud() > 0)
		paresav = this->lista->listarPares();
	else
		paresav = (char *) "< >";

	return paresav;
}

ListaAV * Nodo::getListaAV(){
	return this->lista;
}

void Nodo::setListaAV(ListaAV * l_av){
	this->lista = l_av;
}

void Nodo::eliminarSucesor(int pos){
	//Se debe eliminar el sucesor y parAV en pos, pero solo si no es NORMA!
	this->sucesores->eliminar(pos);
	this->lista->eliminar(pos);
}

void Nodo::eliminarSoloSucesor(int pos){
	this->sucesores->eliminar(pos);
}
