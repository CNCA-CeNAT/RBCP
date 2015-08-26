#include "lista_nodos.h"
#include <cstring>
#include <iostream>

using namespace std;

ListaNodos::NodoL::NodoL(void * nodo, short tipo, char * id){
  this->nodo_ptr = nodo;
  this->tipo = tipo;
  this->id = new char[strlen(id)+1];
  strcpy(this->id, id);
  this->siguiente = 0;
}

short ListaNodos::NodoL::getTipo(){
  return this->tipo;
}

char * ListaNodos::NodoL::getId(){
  return this->id;
}

void * ListaNodos::NodoL::getPtr(){
  return this->nodo_ptr;
}

ListaNodos::NodoL * ListaNodos::NodoL::getSiguiente(){
  return this->siguiente;
}

void ListaNodos::NodoL::setSiguiente(NodoL * sig){
  this->siguiente = sig;
}

ListaNodos::ListaNodos(){
  this->cabeza = 0;
  this->cola = 0;
  this->tam = 0;
}

void ListaNodos::agregar(void * nodo, short tipo, char * id){
  NodoL * temp = new NodoL(nodo, tipo, id);

  //cout << "Lista de sucesores, nuevo nodo con ID: "<<id<<endl;
  //cout << "intentando agregar nodo "<<temp->getId() <<endl;


  if(!cabeza){
	//cout << "agregando a la cabeza..."<<endl;
    cabeza = temp;
    cola = cabeza;
    tam++;
  }
  else{
	  //cout << "Lusta_nodos longitud = "<<this->tam<<endl;
	  //cout << "Cabeza: " <<cabeza->getId() << " - Cola: "<<cola->getId()<<endl;
	  //cout << "agregando al final... "<<cola->getId()<<endl;
    cola->setSiguiente(temp);
    cola = temp;
    tam++;
  }
}

void * ListaNodos::getSucesor(int pos){
  NodoL * temp = cabeza;
  void * suc = 0;

  if(pos < tam){
    while(pos-- != 0) temp = temp->getSiguiente();

    suc = temp->getPtr();
  }

  return suc;
}

int ListaNodos::longitud(){
	return tam;
}

void ListaNodos::eliminar(int pos){
  NodoL * temp = cabeza;
  NodoL * temp2;
  int cont = pos;

  if(pos < tam){
    if(pos == 0){
      cabeza = cabeza->getSiguiente();
      delete temp;
    }
    else{
      while(cont-- > 1) temp = temp->getSiguiente();

      temp2 = temp->getSiguiente();
      temp->setSiguiente(temp2->getSiguiente());
      //Eliminar el borrado para poder "pasar" los nodos de una red a otra
      //delete temp2;

      if(pos == tam-1)  //si se borra la cola
    	  cola = temp;
    }
  
    tam--;
  }
}
