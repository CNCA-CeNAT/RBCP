#ifndef _nodo_
#define _nodo_
#include <cstring>
#include "lista_av.h"
#include "lista_nodos.h"

#define NORMA 100
#define INDICE 200
#define CASO 300

/*
  Clase abstracta NODO.  Es instanciada en forma de Norma, Indice y Caso.
  Contiene una lista enlazada para manejar multiples sucesores, y otra lista para 
  manejar los pares atributo valor según el tipo de nodo.
  * En teoría, lista[i] es el par (a,v) que lleva a sucesores[i].  Se debe hacer explícito?
  * El ID es una hilera de longitud variable.  Se puede hacer fijo y definir una longitud maxima y un "codigo" para generar los ids.
*/

using namespace std;

class Nodo{
  protected:
    short tipo;
    char tipo_txt;
    char id[24];
    ListaNodos * sucesores;
    ListaAV * lista;
  public:
    ~Nodo();
    char * id_nodo();
    short isa();  //Devuelve el num correspondiente a NORMA, INDICE, CASO
    char isa_txt(); //Devuelve N, I, o C
    
    //Sucesores
    void agregarSucesor(Nodo *);
    void eliminarSucesor(int);
    void eliminarSoloSucesor(int);
    Nodo * getSucesor(int);
    int cantSucesores();

	//Pares (a,v)
    void agregarAV(char *, char *);
    int cantParesAV();
    char ** getParAV(int);
    char * verParesAV();  //representacion en texto (lista)
    ListaAV * getListaAV();
    void setListaAV(ListaAV *); //Copiar una lista (a,v) completa
    
};

#endif
