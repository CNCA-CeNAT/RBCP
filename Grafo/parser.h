/*
 * ***************************
 * Parser de Lista<->Red
 * ***************************
 * 
 * AUTOR: Juan Carlos Saborío Morales
 * ORGANIZACIÓN: Colaboratorio Nacional de Computación Avanzada
 * FECHA: 27/07/2009
 * MODIFICACION: 01/12/2009, para utilizar la nueva clase "Grafo" (nodos tipo Caso, Indice, Norma) con listas de atributos.
 * MODIFICACION: 10/12/2009, para utilizar la clase 'Red' en lugar de Grafo.
 * 
 * Procesa la lista que representa una red de discriminación redundante y permite:
 * 
 * 1- Obtener el subarbol (todos los descendientes incluyendo el padre) para un nodo a partir de su etiqueta
 * 2- Obtener la cantidad y nombre de los sucesores inmediatos a un nodo a partir de su etiqueta
 * 3- Generar una estructura de datos "Red" (para realizar búsquedas) a partir de una lista
 * 4- Cargar una lista que corresponde a una red a partir de un archivo de texto
 * 5- Obtener la etiqueta de la raiz de la red
 * 
 * */

#ifndef PARSER_H_
#define PARSER_H_
#include "../red.h"

class Parser{

	public:
		Parser();
		//void setDatos(char *);
		//int cantSucesores(char *); //Cantidad de sucesores inmediatos del nodo con etiqueta X
		char * subArbol(char *, char *); //Obtener el subarbol a partir del nodo con etiqueta X (arbol-lista, etiqueta)
		char ** sucesoresNodo(char *, char *);  //Retorna un arreglo con la cantidad de sucesores [0] y el nombre de cada uno de ellos [1...n] : (arbol-lista, etiqueta)
		Red * generarGrafo(char *);  //Construye la estructura de Grafo a partir de una lista
		//char * generarLista(Grafo *);  //Construir la representacion de lista para el Grafo G
		char * leerArchivo(char *); //lee la lista contenida en un archivo de texto
		char * obtenerRaiz(char *); //obtiene la etiqueta del nodo raiz para una lista

		/*Funciones adicionales para balanceo de cargas*/
		int contarNodos(char *); //Contar la cantidad de nodos en toda la red
		char ** dividir(char *); //Dividir la lista en dos mitades, retorna dos arreglos, [0] es la izquierda y [1] la derecha
		int cantidadSubarboles(char *); //Obtener la cantidad de sucesores de la raiz
		int redVacia(char *);  //Determinar si una "red" tiene nodos ademas de la raiz
};

#endif /*PARSER_H_*/
