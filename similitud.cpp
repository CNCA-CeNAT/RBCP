#include "similitud.h"
#include "cargarCasos.h"
#include <cstring>
#include <math.h>
#include <cstdlib>
#include <iostream>

Similitud::Similitud(){

	//Inicializar los meses
	meses[0] = "January";
	meses[1] = "February";
	meses[2] = "March";
	meses[3] = "April";
	meses[4] = "May";
	meses[5] = "June";
	meses[6] = "July";
	meses[7] = "August";
	meses[8] = "September";
	meses[9] = "October";
	meses[10] = "November";
	meses[11] = "December";

	//Inicializar las estaciones
	estaciones[0] = "Winter";
	estaciones[1] = "Spring";
	estaciones[2] = "Summer";
	estaciones[3] = "Autumn";
}

/* Calcular la similitud para dos valores que corresponden a un nodo de la red y un problema.
 * v1 es el valor en el nodo actual
 * v2 es el valor en el problema
 *
 * */

int Similitud::sim(char * v1, char * v2, char * atributo, int tipo){
	int sim = 0;

	switch(tipo){
		case STRING:
			//Si la distancia.  +- 1 se toma como valido
			//cout << "Evaluando hileras"<<endl;
			if(diferenciaMeses(v1,v2) < 2){
				//cout << "son similares"<<endl;
				sim = 1;
			}
			break;
		case ENTERO:
			if(strcmp(atributo, "NumberOfPersons") == 0)
				sim = (atoi(v2) == atoi(v1)-1 || atoi(v2) == atoi(v1)+1);
			else
				if(strcmp(atributo, "Duration") == 0)
					sim = (atoi(v2) > atoi(v1)-3 || atoi(v2) < atoi(v1)+3);
			break;
		case DECIMAL:
			if(strcmp(atributo, "Price"))
				sim = (atoi(v1)*0.9 <= atoi(v2) && atoi(v2) <= atoi(v1)*1.1);
			break;
	}

	return sim;
}

/*
 * Calcula la distancia entre dos meses conviertiendo el arreglo en "circular".
 * Si un valor no es mes, su "posicion" es 999 y la diferencia es tan grande que no supera el criterio, i.e. son muy diferentes.
 *
 * */
int Similitud::diferenciaMeses(char * m1, char * m2){
	int pos_m1=-1;
	int pos_m2=-1;
	int dif=999;

	for(int i=0;i<12 && (pos_m1==-1 || pos_m2 == -1);i++){
		if( strcmp(meses[i],m1) == 0)
			pos_m1 = i;
		if( strcmp(meses[i],m2) == 0)
			pos_m2 = i;
	}

	if(pos_m1 != -1 && pos_m2 != -1)
		dif = abs(pos_m1 - pos_m2)%11;

	//cout << "Dif meses ="<<dif<<endl;

	return dif;
}

/*
 * Calcula la distancia entre dos estaciones conviertiendo el arreglo en "circular".
 * En realidad no se usará ya que las estaciones son muy diferentes entre si...!
 *
 * */
int Similitud::diferenciaEstaciones(char * e1, char * e2){
	int pos_e1=-1;
	int pos_e2=-1;
	int dif=999;

	for(int i=0;i<4 && (pos_e1==-1 || pos_e2 == -1);i++){
		if( strcmp(estaciones[i],e1) == 0)
			pos_e1 = i;
		if( strcmp(estaciones[i],e2) == 0)
			pos_e2 = i;
	}

	if(pos_e1 != -1 && pos_e2 != -1)
		dif = abs(pos_e1 - pos_e2)%11;

//	cout << "Dif estaciones ="<<dif<<endl;

	return dif;
}
