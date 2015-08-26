#ifndef SIMILITUD_H
#define SIMILITUD_H

using namespace std;

class Similitud{
	private:
		//arreglo circular de meses
		char * meses[12];
		char * estaciones[4];
		int diferenciaMeses(char *, char *);
		int diferenciaEstaciones(char *, char *);

	public:
		Similitud();
		int sim(char *, char *, char *, int); //Calcular similitud entre los valores X e Y, que corresponden al atributo Z

};

#endif
