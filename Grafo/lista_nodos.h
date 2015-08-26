#ifndef lista_nodos
#define lista_nodos
/*
  Lista enlazada que implementa los sucesores de un "nodo" en el grafo, que pueden
ser Casos, Indices o Normas.  Se utiliza otra clase NodoL para evitar la recurrencia ya que 
ListaNodos esta presente dentro de la clase Nodo.
*/

class ListaNodos{
  private:
    class NodoL{
      private:
		void * nodo_ptr;  //Puntero a un nodo del GRAFO, convertido a VOID para evitar doble referencia
		short tipo;  //Tipo de nodo al que corresponde: CASO, NORMA, INDICE
		char * id;  //Valor de identificacion del nodo en el GRAFO
		NodoL * siguiente;
      public:
		NodoL(void *, short, char *);
		short getTipo();
		char * getId();
		void * getPtr();
		NodoL * getSiguiente();
		void setSiguiente(NodoL *);
    };
	NodoL * cabeza;
	NodoL * cola;
    int tam;

  public:
    ListaNodos();
    void agregar(void *, short, char *);
    void * getSucesor(int);
    int longitud();
    void eliminar(int);
    void verContenido();

};

#endif
