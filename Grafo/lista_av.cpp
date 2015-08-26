#include "lista_av.h"
#include <iostream>
#include <cstring>
#include <sstream>

using namespace std;

ListaAV::NodoAV::NodoAV(char * atributo, char * valor, int tipo){
  this->atributo = new char[strlen(atributo)+1];
  strcpy(this->atributo, atributo);

  this->valor = new char[strlen(valor)+1];
  strcpy(this->valor, valor);

  this->tipo = tipo;

  this->siguiente = 0;
}

char * ListaAV::NodoAV::getAtributo(){
  return this->atributo;
}

char * ListaAV::NodoAV::getValor(){
  return this->valor;
}

void ListaAV::NodoAV::setAtributo(char * atributo){
  this->atributo = new char(strlen(atributo));
  strcpy(this->atributo, atributo);
}

void ListaAV::NodoAV::setValor(char * valor){
  this->valor = new char(strlen(valor));
  strcpy(this->valor, valor);
}

void ListaAV::NodoAV::setSiguiente(NodoAV * sig){
  this->siguiente = sig;
}

ListaAV::NodoAV * ListaAV::NodoAV::getSiguiente(){
  return this->siguiente;
}

//Funciones de la listaAV

int ListaAV::NodoAV::getTipo(){
  return this->tipo;
}

ListaAV::ListaAV(){
  this->tam = 0;
  cabeza = 0;
  cola = 0;
}

//Agregar nodo al final de la lista
int ListaAV::agregar(char * atributo, char * valor, int tipo){
  NodoAV * temp = new NodoAV(atributo, valor, tipo);
  int exito = -1;

  if(!cabeza){
    cabeza = temp;
    cola = temp;
    exito = 1;
    tam++;
  }
  else{
    cola->setSiguiente(temp);
    cola = temp;
    exito = 1;
    tam++;
  }

  return exito;
}

//Eliminar un nodo segun su posicion
int ListaAV::eliminar(int posicion){
  NodoAV * temp = cabeza;
  NodoAV * temp2;
  int exito = -1;
  int i=0;
  
  if(posicion == 0){
  	//Borrar la cabeza
  	cabeza = temp->getSiguiente();
  	delete temp;
  	tam--;
  	exito = 1;
  }
  else{
  	while(i++ < posicion-1) temp = temp->getSiguiente();
	
	temp2 = temp->getSiguiente();

	if(posicion == tam-1){
		//Borrar la cola
		temp->setSiguiente(0);
		cola = temp;
	}
	else{
		temp->setSiguiente(temp2->getSiguiente());
	}
	
	delete temp2;
	tam--;
	exito = 1;
  }
  
  return exito;
}

char * ListaAV::listarPares(){
	NodoAV * temp = cabeza;
	//char * lista = new char[tam*20*2];  //tam * (atr + val)
	ostringstream lista;
	
	//sprintf(lista, "< ");
	lista << "<";
  	for(int i=0; i<tam; i++){
		//sprintf(lista, "%s(%s,%s) ",lista,temp->getAtributo(),temp->getValor());
  		lista << "(" << temp->getAtributo() << "," << temp->getValor() << ")";
		temp = temp->getSiguiente();
    }
    //sprintf(lista, "%s>", lista);
  	lista << ">";

  	int len = strlen(lista.str().c_str());
    char * lista_c = new char[len+1];
   	strcpy(lista_c, lista.str().c_str());
//   	lista_c[len] = '\0';

   	//for(int i=0;i<strlen(lista_c);i++) cout << "lista_c["<<i<<"] = "<<lista_c[i]<<endl;

//   	cout << endl;
    
    return lista_c;
}

void ListaAV::verContenido(){
  NodoAV * temp = cabeza;
  int i=0;
  
  cout << "<";
  while(i++ < tam){
    cout << "(" << temp->getAtributo() << ", "<<temp->getValor() << ")";
    temp = temp->getSiguiente();
  }
  cout << ">";
}

int ListaAV::longitud(){
	return tam;
}

char ** ListaAV::getParAV(int pos){
	NodoAV * temp = cabeza;
	
	while(pos-- > 0 && pos <= tam) temp = temp->getSiguiente();
	
	char ** resultado = new char*[2];
	resultado[0] = new char[strlen(temp->getAtributo())+1];
	resultado[1] = new char[strlen(temp->getValor())+1];
	
	strcpy(resultado[0], temp->getAtributo());
	strcpy(resultado[1], temp->getValor());
	
	return resultado;
}

int ListaAV::existeAtributo(char * atr){
	int pos = -1;
	NodoAV * temp = cabeza;

	for(int i=0; i<tam && pos == -1; i++){
		if( strcmp(temp->getAtributo(),atr) == 0 ){
			pos = i;
		}
		temp = temp->getSiguiente();
	}

	return pos;
}

int ListaAV::existeAtributoValor(char ** parAV){
	int existe = 0;

	NodoAV * temp = cabeza;

	for(int i=0; i<tam && !existe; i++){
		if(strcmp(temp->getAtributo(),parAV[0]) == 0 && strcmp(temp->getValor(),parAV[1]) == 0){
			existe = 1;
		}
		temp = temp->getSiguiente();
	}

	return existe;
}
