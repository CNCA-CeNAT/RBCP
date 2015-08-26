#include <fstream>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "cargarCasos.h"

using namespace std;

CargarCasos::CargarCasos(){
	casosM = 0;
	cant_columnas = 0;
	cant_casos = 0;
}

void CargarCasos::leerBD(){
	/*
    char * db = "Casos", * server = "localhost", *user = "root", *pass = "root";
	
	try {                       
        // Establish the connection to the database server.
        mysqlpp::Connection con(db, server, user, pass);

        // Retrieve a subset of the stock table's columns, and store
        // the data in a vector of 'stock' SSQLS structures.  See the
        // user manual for the consequences arising from this quiet
        // ability to store a subset of the table in the stock SSQLS.
        mysqlpp::Query query = con.query("select item,description from stock");
        vector<stock> res;
        query.storein(res);

        // Display the items
        cout << "We have:" << endl;
        vector<stock>::iterator it;
        for (it = res.begin(); it != res.end(); ++it) {
            cout << '\t' << it->item;
            if (it->description != mysqlpp::null) {
                cout << " (" << it->description << ")";
            }
            cout << endl;
        }
    }
    catch (const mysqlpp::BadQuery& er) {
        // Handle any query errors
        cerr << "Query error: " << er.what() << endl;
        return -1;
    }
    catch (const mysqlpp::BadConversion& er) {
        // Handle bad conversions; e.g. type mismatch populating 'stock'
        cerr << "Conversion error: " << er.what() << endl <<
                "\tretrieved data size: " << er.retrieved <<
                ", actual size: " << er.actual_size << endl;
        return -1;
    }
    catch (const mysqlpp::Exception& er) {
        // Catch-all for any other MySQL++ exceptions
        cerr << "Error: " << er.what() << endl;
        return -1;
    }
*/
}

int CargarCasos::leerArchivo(char * archivo){
	//lista_caso * casos;
	int contador = 0;
	int k=0, j=0;
	int cant_cols=0;
	int cant_dims=0;
	
	char nom_tipos[10];

	char datos[256];
	char ** temp;
	ifstream abrir;
	abrir.open(archivo);
	if(!abrir){              
		cerr << "Error al abrir archivo \""<<archivo<<"\".\n"<<endl;
		return -1;
	}                                                             
	else{
		//cout << "Abriendo archivo \""<< archivo<<"\" ";
		//Leer cantidad de casos:
		abrir.getline(datos,256);
		this->cant_casos = atoi(datos)+1;  //Se usa una fila más para almacenar el nombre de las dimensiones
		//cout << "con " << cant_casos-1 << " casos."<<endl;

		//Leer cantidad de dimensiones
		abrir.getline(datos,256);
		cant_dims = atoi(datos);

		temp = new char*[cant_dims];

		for(int i=0; i<cant_dims;i++)
			temp[i] = new char[MAX_LEN];


		//Leer y guardar los tipos de datos
		abrir.getline(datos,256);
		tipos = new int[cant_dims];


		for(int i=0, cont=0, tipo_actual=0; datos[i] != '#'; i++){
			if(datos[i] == ';'){
				nom_tipos[cont] = '\0';
				if(strcmp(nom_tipos, "STRING") == 0)
					tipos[tipo_actual++] = STRING;
				else if(strcmp(nom_tipos, "INT") == 0)
					tipos[tipo_actual++] = ENTERO;
				else if(strcmp(nom_tipos, "REAL") == 0)
					tipos[tipo_actual++] = DECIMAL;

				cont=0;
			}
			else
				nom_tipos[cont++] = datos[i];
		}

		//Crear la matriz de casos
		casosM = new char**[cant_casos];

		//abrir.getline(datos,256); //nombres de las columnas
		while(!abrir.eof() && contador < cant_casos){
			abrir.getline(datos,256);
			k=0;
			j=0;
			//cout << datos << ", " << contador <<endl;
			//procesar la linea (tokenizar)
			cant_cols=0;
			for(int i=0;datos[i] != '#'; i++){
				if(datos[i] != ';'){
					temp[j][k++] = datos[i];
				}
				else{
					temp[j][k] = '\0';
					//cout << " concatenado: "<<temp[j]<<endl;
					j++;
					k=0;
					cant_cols++;
				}
			}
			cant_cols++;
			temp[j][k] = '\0';
			//cout << " concatenado: "<<temp[9]<<endl;
			//copiar los datos a la posicion en "casos"	
			casosM[contador] = new char*[cant_cols];
			
			for(int i=0; i<cant_cols;i++){
				casosM[contador][i] = new char[strlen(temp[i])+1];
				strcpy(casosM[contador][i],temp[i]);
			}
			
			contador++;

			
		}

		this->cant_columnas = cant_cols;

		//cout << cant_casos-1 << " cargados exitosamente."<<endl;
	}
	return 0;
}

char *** CargarCasos::getCasosM(){
	return this->casosM;
}

char ** CargarCasos::nombreColumnas(){
	/*char ** columnas = new char *[9];
	
	columnas[0] = "HolidayType";
	columnas[1] = "Price";
	columnas[2] = "NumberOfPersons";
	columnas[3] = "Region";
	columnas[4] = "Transportation";
	columnas[5] = "Duration";
	columnas[6] = "Season";
	columnas[7] = "Accommodation";
	columnas[8] = "Hotel";*/
	 
	return casosM[0];
}

int CargarCasos::cantidadColumnas(){
	return this->cant_columnas;
}

int CargarCasos::cantidadCasos(){
	return (this->cant_casos-1);
}

int CargarCasos::getTipo(char * nombre_dimension){

	int tipo=-1;

	for(int i=0;i<this->cant_columnas && tipo==-1;i++)
		if(strcmp(casosM[0][i], nombre_dimension) == 0)
			tipo = tipos[i];

	return tipo;
}
