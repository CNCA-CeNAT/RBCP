/*
 * Simplemente una Pila.
 * 
 * Mantiene la estructura de nodo (etiqueta,valor) igual que el Grafo.
 * 
 * TODO: Se podría hacer la clase Nodo publica para que la utilicen el Arbol/Grafo y la Pila. 
 * 
 * */
 
#ifndef PILA_H_
#define PILA_H_

class Pila{
	private:
		class Nodo{
		    private: 
		      long valor;
		      char * etiqueta;
		      Nodo * siguiente;
		    public:            
		      Nodo();    
		      Nodo(long, char *);
		      long getValor();
		      char * getEtiqueta();
		      Nodo * getSiguiente();
		      void setSiguiente(Nodo *);
		  };
		Nodo * cabeza;
		int tam;
		
	public:
		Pila();
		void push(long, char *);
		char * topE();
		long topV();
		void pop();
		int length();
		int vacia();
		void listar();
		
};

#endif /*PILA_H_*/
