/*
 * ********************************
 * Red de Discriminación Redundante
 * ********************************
 *
 * AUTOR: Juan Carlos Saborío Morales
 * ORGANIZACIÓN: Colaboratorio Nacional de Computación Avanzada
 * FECHA: 02/03/2010
 *
 * Estructura de datos para representar la memoria de casos.
 * Funciones:
 *
 * 1- Generar una nueva "Red" a partir de una coleccion de casos previamente elaborada.
 * 2- Generar una representación de sí misma en forma de lista, que puede ser distribuida mediante MPI.
 *
 * */

#ifndef RED_H_
#define RED_H_
#include "Grafo/nodo.h"
#include "Grafo/norma.h"
#include "Grafo/indice.h"
#include "Grafo/caso.h"
#include "cargarCasos.h"
#include "similitud.h"
#include <cstdlib>
#include <sstream>

#define LARGO_PROBLEMA 128
#define LARGO_DIMENSION 128

using namespace std;

class Red{
	private:
		Norma * raiz;
		CargarCasos * cg;
		char *** base_de_casos;
		int cantidad_total_de_casos;
		char ** nombre_columnas;
		int cantidad_de_columnas;

		int cantidad_total_de_nodos;
		time_t t; //Semilla para numeros aleatorios
		//fstream salida; //Para generar archivo de estadisticas

		/* Buscar la norma donde se debe indizar un caso */
		Norma * buscarNorma(char **); //Funcion "interfaz" para buscar una norma
		Norma * buscarNormaR(Nodo *, char **, double);  //Implementacion recursiva de buscarNorma

		/* Calcular la similitud entre un nodo cualquiera y un caso usando el algoritmo de Tversky */
		Similitud * sim_viajes;
		double similitud(Nodo *, char **);
		double sim_f(char *, char *, int);  //Funcion que calcula similitud para dos atributos dependiendo del tipo (String, int, real)

		/* Encontrar la posicion de un caso en la coleccion a partir de su id (JourneyCode) */
		int posicionBaseDeCasos(char *);

		/* Calcular las dimensiones para indizar un caso */
		ListaAV * calcularDimensiones(char **, ListaAV *); //Genera una lista de pares (a,v) para indizar un caso, excluyendo los pares contenidos en la lista parametro
		ListaAV * calcularDimensiones(char **); //Retorna todas las dimensiones de un caso

		/* Generar una etiqueta automaticamente */
		void generarID(char, char **, int, int, char *);  //Generar un valor a partir del tipo de nodo, un par (a,v) y dos enteros.  Escribe la etiqueta en una hilera.


		/*** Funciones recursivas auxiliares ***/
		void verContenidoR(Nodo *, int, fstream *);  //Desplegar contenido de la red en formato legible
		void listaR(Nodo *, ostringstream *); //Generar representacion de lista
		int agregarR(Nodo *, char *, Nodo *); //Agregar un nodo al nodo con etiqueta 'id'

		/** Generacion de la red **/
		void generarR(char ***, int, Norma *);  //Generación recursiva de la red, llamado por "generar"

		/*** Borrado recursivo de la red ***/
		void BorrarRed(Nodo *);

		int contar_nodos_R(Nodo *);

		int nodinho; //solo pruebas!

		/*** Algoritmo de busqueda - TENTATIVO ***/
		void buscarR(Nodo *, char **, ostringstream *, ListaAV *);  //buscar recursivo de problemas
		void buscarEx(Nodo *, char **, Nodo **);  //buscar exhaustivo

		/* Para ejecutar la recuperacion desde "afuera" */
		Nodo * nodo_actual;


		/**************************/
		/* Funciones 'itoa' y 'reverse' tomadas de: Kernighan & Ritchie: "The C Programming Language".  2º Edicion, (1988). pp. 64
		*/
		void reverse(char []);  //Utilizado por 'itoa' para invertir la hilera generada
		void itoa(int, char []);  //Convierte un 'int' en un arreglo de 'char'
		/**************************/

		/*
		 * AUX pruebas y analisis
		 * */
		void calcular_factorR_rec(Nodo *, int *, int *);

	public:
		Red();
		~Red();
		int generar(char *);  //Genera una red de discriminación a partir de una coleccón de casos (archivo de texto)
		void probar();
		void verContenido();  //Despliega el contenido en forma de texto en la salida estandar
		void verContenido(char *);  //Despliega el contenido en forma de texto en un archivo
		char * generarLista();  //Genera la lista correspondiente al contenido actual de la red
		int agregar(Nodo *, char *);  //Utilizado por el PARSER para generar una red nueva a partir de una lista preexistente
		
		int tamano();

		Nodo * getRaiz();
		void sumarContadorNodos();

		/*
		 * Funciones de prueba y analisis
		 *
		 * */
		int contar_nodos();
		float calcular_factorR();  //Calcular numericamente el factor de ramificacion


		/*
		 * Funciones de busqueda
		 * */
		char * buscar(char [][LARGO_PROBLEMA], char);  //retornar la ruta solucion (si esta existe) para una descripcion de un problema.  El segundo parametro es el tipo de busqueda ('H'euristica o 'E'xhaustiva)
		Nodo * nodoActual();
		void nodoSiguiente(Nodo *);

		/*
		 * Para solucionar la distribucion de la matriz de casos
		 * */
		//char *** getMatrizCasos();
		int getCantidadCasos();
		int getCantidadDimensiones();
		char ** getNombreDimensiones();
		void setDimensiones(char [][LARGO_DIMENSION], int);  //Asignar el nombre y cantidad de dimensiones, usado por funciones internas


		/*
		 * Modificacion estructural.  Para balanceo de cargas dinamico
		 * Se debe dividir la red en 2 o mas subredes, a partir de un nodo compartido, cada red con ~la misma cantidad de sucesores
		 *
		 * */
		Red * dividir();
};

#endif /*RED_H_*/
