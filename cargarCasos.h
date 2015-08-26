#ifndef CARGARCASOS_
#define CARGARCASOS_

/*
 * Lee todos los casos en la colección.
 * Genera una estructura fácilmente utilizable en C/C++ 
 * */

#define STRING	0
#define ENTERO	1
#define DECIMAL	2
#define MAX_LEN 100 //Tamaño máximo de hilera en caracteres
/*
typedef struct{
  char JourneyCode[6];
  char HolidayType[50];
  char Price[6];
  char NumberOfPersons[3]; 
  char Region[50];
  char Transportation[50];
  char Duration[4];
  char Season[50];        
  char Accommodation[50]; 
  char Hotel[100];        
} lista_caso;
*/

class CargarCasos{
	
	private:
		char *** casosM;
		int * tipos;
		int cant_columnas;
		int cant_casos;
	
	public:
		CargarCasos();
		void leerBD();
		int leerArchivo(char *);
		char ** nombreColumnas();
		int cantidadColumnas();
		int cantidadCasos();
		char *** getCasosM();
		int getTipo(char *);
};

#endif /*CARGARCASOS_*/
