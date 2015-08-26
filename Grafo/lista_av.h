#ifndef listaAV
#define listaAV
// Lista de pares atributo-valor para los nodos del grafo

class ListaAV{
  private:
  class NodoAV{
    private:
      char * atributo;
      char * valor;
      int tipo;
      NodoAV * siguiente;

    public:
      NodoAV(char *, char *, int);
      char * getAtributo();
      void setAtributo(char *);
      char * getValor();
      void setValor(char *);
      int getTipo();

      void setSiguiente(NodoAV *);
      NodoAV * getSiguiente();
  };
  NodoAV * cabeza;
  NodoAV * cola;
  int tam;
  
  public:
    ListaAV();
    int agregar(char *, char *, int); /* atributo, valor, tipo de nodo*/
    int eliminar(int);
    int longitud();
    char ** getParAV(int);
    char * listarPares();  //listar en texto (lista)
    void verContenido();
    int existeAtributo(char *);  //Determina si existe o no el atributo mencionado dentro de la lista.  Retorna su posicion o -1.
    int existeAtributoValor(char **); //Determina si existe el par (a,v) ([0][1]) en la lista.
};

#endif
