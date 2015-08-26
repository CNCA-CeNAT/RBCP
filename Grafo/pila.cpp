#include "pila.h"
#include <iostream>

using namespace std;

Pila::Nodo::Nodo(){
	this->valor = -1;
	this->etiqueta = "";
}    

Pila::Nodo::Nodo(long val, char * et){
	this->valor = val;
	this->etiqueta = et;
	this->siguiente = 0;
}

long Pila::Nodo::getValor(){
	return this->valor;
}

char * Pila::Nodo::getEtiqueta(){
	return this->etiqueta;
}

Pila::Nodo * Pila::Nodo::getSiguiente(){
	return this->siguiente;
}

void Pila::Nodo::setSiguiente(Nodo * sig){
	this->siguiente = sig;
}
		      
		      
Pila::Pila(){
	this->tam = 0;
	this->cabeza = 0;
}

void Pila::push(long val, char * etiq){
	Nodo * temp = new Nodo(val, etiq);
	
	if(!cabeza){
		cabeza = temp;
		tam++;
	}
	
	else{
		temp->setSiguiente(cabeza);
		cabeza = temp;
		tam++;
	}
	
}

char * Pila::topE(){
	if(!vacia())
		return cabeza->getEtiqueta();
	else
		return "";
}

long Pila::topV(){
	if(!vacia())
		return cabeza->getValor();
	else
		return 0;
}

void Pila::pop(){
	Nodo * temp = cabeza;
	cabeza = cabeza->getSiguiente();
	
	delete temp;
	
	tam--;
}

int Pila::length(){
	return tam;
}

int Pila::vacia(){
	//cout << (tam == 0) <<endl;
	return (tam == 0);
}

void Pila::listar(){
	Nodo * temp = cabeza;
	
	cout <<"Pila tam = "<<tam<<endl;
	
	while(temp != 0){
		cout << "("<<temp->getEtiqueta() << ", "<<temp->getValor()<<")";
		temp = temp->getSiguiente();
	}
	cout << endl;
}
