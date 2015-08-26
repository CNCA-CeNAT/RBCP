#include "parser.h"
#include "pila.h"
#include "math.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>


using namespace std;

Parser::Parser(){
	//this->datos = "";
}


char * Parser::subArbol(char * datos, char * etiqueta){
/* Obtener el subarbol a partir del nodo con etiqueta X
 *
 * Retorna el subarbol (conjunto de nodos a partir del nodo con etiqueta X, incluyendolo) que consiste en un
 * "substring" dentro de la hilera datos.
 *  */

	char * subarbol;
	int pars = 0;
	int pars_nodo = 0;
	int largo = strlen(datos);
	char temp[256];
	int encontrado = 0;
	int inicio = 0, fin = 0;
	int tipo = 0;
	
	//cout << "Procesando arbol: " << datos<<endl;
	
	for(int i=0; i<largo && !encontrado; i++){
		//cout << datos[i]<<endl;
		switch(datos[i]){
			case '(':
				pars++;
				break;
			case ')':
				pars--;
				break;
			/*case 'N':
				tipo = NORMA;
				break;
			case 'I':
				tipo = INDICE;
				break;
			case 'C':
				tipo = CASO;
				break;*/
			case '[':
				//lo que sigue es el ID del nodo, termina con ]		
				strcpy(temp,"");
				int j, k;
				for(j=0, k=i+1;k<largo && datos[k] != ']';k++, j++){  //+1 se salta el [
					temp[j] = datos[k];
				}
				temp[j] = '\0';
				//cout << temp<<endl;
				//revisar si temp == etiqueta, y empezar aqui
				if(strcmp(temp,etiqueta)==0){
					//cout << "etiqueta es el objetivo... procesar resto."<<endl;
					//cargar todo_, mientras se mantenga la misma cantidad de parentesis
					inicio = i-1;  //incluir el tipo
					pars_nodo = pars;
					
					for(fin=i; fin<largo && pars_nodo >= pars;fin++){
						//Contar los parentesis
						switch(datos[fin]){
							case '(':
									pars_nodo++;
								break;
							case ')':
									pars_nodo--;
								break;
						}
					}
					//cout << "inicio = "<<inicio<<", fin = "<<fin<<endl;
					encontrado = 1;
				}
				break;
		}
	}
	
	largo = fin-inicio;
	subarbol = new char[largo+2];
	
	strcpy(subarbol,"");
	
	//subarbol[0] = '(';
	int j=0;
	for(int i=inicio-1; i<=fin && j<largo; i++, j++){
		subarbol[j] = datos[i];
	}

	subarbol[j++]=')';
	subarbol[j] = '\0';  //Agregar terminacion de linea para evitar memleak de strlen

	return subarbol;
}

char ** Parser::sucesoresNodo(char * datos, char * etiqueta){
/*
 * Obtener los nombres de los sucesores inmediatos al nodo con etiqueta X
 * 
 * Retorna una "matriz" o un arreglo de hileras de texto.
 * La primera posición de la matriz es un entero "casteado" a char *, y es la longitud del resto del arreglo.
 * */	
	int cantidad = 0;  //this->cantSucesores(etiqueta);
	char ** sucs; //= new char * [cantidad];
	int largo = strlen(datos);
	char * temp;
	int encontrado = 0;
	int pars = 0, pars_nodo = 0;
	Pila * nodos = new Pila();
	
	for(int i=0; i<largo && !encontrado; i++){
		//cout << datos[i]<<endl;
		switch(datos[i]){
			case '<':
				//saltarse toda la lista de atributos
				//cout << "saltando lista de atributos..."<<endl;
				for(; datos[i] != '>'; i++);
				break;
			case '(':
				pars++;
				break;
			case ')':
				pars--;
				break;
			case '[':
				//lo que sigue es una etiqueta, termina con ']'		
				temp = new char[256];
				int j, k;
				for(j=0, k=i+1;k<largo && datos[k] != ']';k++, j++){  //k=i+1 se salta el [
					temp[j] = datos[k];
				}
				temp[j] = '\0';
				//cout << temp<<endl;
				//revisar si temp == etiqueta, y empezar aqui
				//cout << "pars para etiq \""<< temp <<"\": "<<pars<<endl;
				if(strcmp(temp,etiqueta)==0){
					//cout << "etiqueta es el objetivo... procesar resto."<<endl;
					//BUSCAR Sucesores para este nodo
					
					//ubicarse en el primer parentesis despues de la lista de atributos!!!
					for(;k<largo && datos[k] != '>'; k++);
					//buscar el parentesis
					for(;k<largo && datos[k] != '('; k++);
					k++; //pasar el parentesis
					pars++;  //parentesis nuevo
					pars_nodo = pars; //esta cantidad se debe mantener
					
					//cout << "Cantidad de parentesis para nodo \""<<temp<<"\": "<<pars<<endl;
					
					while(pars_nodo == pars || pars_nodo == pars+1){
						//si se abre un parentesis debe seguir un nodo
						//buscar []'s y sacar la etiqueta
						switch(datos[k]){
							case '<':
								//saltarse toda la lista de atributos
								for(; datos[k] != '>'; k++);
								break;
							case '(':
									pars_nodo++;
									//si se abre otro despues de una etiq (parsnodo == pars+2), entonces son sucesores de este otro nodo, saltar!!			
									if(pars_nodo == pars+2){
										//cout << "subnodos dentro del sucesor...saltando"<<endl;
										k++; //moverse a la siguiente posicion
										while(k<largo && (pars_nodo >= pars+2)){
											switch(datos[k]){
												case '<':
													//saltarse toda la lista de atributos
													for(; datos[k] != '>'; k++);
													break;
												case '(':
														pars_nodo++;
													break;
												case ')':
														pars_nodo--;
													break;
											}
											k++;
										}
										if(datos[k] == ')') pars_nodo--; //contar el parentesis actual
										//cout << "ahora pars_nodo es: "<<pars_nodo<<endl;
										//cout << "apuntando a la posicion: " << k << "["<<datos[k]<<"]"<<endl;
									}
								break;
							case ')':
									pars_nodo--;
								break;
							case '[':
									temp = new char[64];
									k++; //+1 se salta el [
									//cout << "K: "<<k<<endl;
									for(j=0; k<largo && datos[k] != ']';k++, j++){ 
										temp[j] = datos[k];
									}
									temp[j] = '\0';
									//temp tiene la etiqueta de un nodo
									nodos->push(0, temp);  //agregarlo a la pila
									
									//cout << "Agregado a la pila nodo \""<< temp<<"\""<<endl;									
								break;
						}
						k++;
					}
					
					//cout << "inicio = "<<inicio<<", fin = "<<fin<<endl;
					encontrado = 1;
				}
				break;
		}
	}
	
	cantidad = nodos->length();
	
	sucs = new char *[cantidad+1];
	
	sucs[0] = (char *) cantidad;	
	
	for(int i=1;i<=cantidad;i++){
		sucs[i] = nodos->topE();
		nodos->pop();
	}
	
	return sucs;
}

Red * Parser::generarGrafo(char * datos){
/*
 * Generar la estructura de Arbol/Grafo a partir de la lista (texto)
 * 
 * Se debe leer una lista e ir generando/insertando los nodos (valor,etiqueta, padre) en la red.
 * 
 * NOTA: Una red nueva ya tiene una norma raíz, por lo que hay que usar esta en lugar de la raíz de la lista.
 *
 * */
 
	
	Pila * nodos = new Pila();
	Pila * pars = new Pila();
	Red * g = new Red();
	
	Nodo * n;
	Nodo * nodo_padre;
	
	char * etiqueta;
	//char * tempVal;
	char * padre;
	int j, k = -1;
	int largo = strlen(datos);
	int parentesis = 0;
	int tipo = 0;
	char tipo_c;
	char * atributo;
	char * valor;
	
	//cout << "datos: " << datos<<endl;
	nodos->push( (long) g->getRaiz(), "raiz");  //SE NECESITA PARA CREAR SUBARBOLES!
	
	for(int i=0; i<largo;i++){
		//cout << "pila " << nodos->length() <<endl; 
		//cout << "datos["<<i <<"]"<<datos[i]<<", siguiente: "<<datos[i+1]<<endl;
		switch(datos[i]){
			case '(':
				//Se abre parentesis, necesariamente sigue un nodo...
				parentesis++;
				etiqueta = new char[20];
				//tempVal = new char[10];
				atributo = new char[32];
				valor = new char[64];
				
				//1. Obtener el tipo de nodo
											
				//2. Obtener el ID del nodo
				// buscar [
				for(j=i+1; j<largo && datos[j] != '['; j++){
					if(datos[j] == '(')
						parentesis++;
				}
				tipo_c = datos[j-1]; //Antes del [ esta el tipo
				j++; //saltar [

				//Concatenar la etiqueta/id
				for(k=0; j<largo && datos[j] != ']'; j++,k++){
					etiqueta[k] = datos[j];
				}
				etiqueta[k] = '\0';
				j++; //saltarse el ]
				

				//3. Crear nodo
				//cout << "etiqueta: "<<etiqueta<<endl;
				//cout << "Nodo tipo: " << tipo_c << " con id \"" << etiqueta << "\""<<endl;
				switch(tipo_c){
					case 'N':
						if(strcmp(etiqueta,"raiz")==0){
							n = g->getRaiz();
						}
						else{
							tipo = NORMA;
							n = new Norma(etiqueta);
						}
						break;
					case 'I':
						tipo = INDICE;
						n = new Indice(etiqueta);
						break;
					case 'C':
						//cout << "creando caso..."<<endl;
						tipo = CASO;
						n = new Caso(etiqueta);
						break;
					default:
						cout << "ERROR: Tipo no corresponde!!"<<endl;
						cout << "Tipo: "<<tipo_c<<", etiqueta: "<<etiqueta<<", posicion: "<<i<<endl;
						cout << "LISTA: ";
						for(int k=-15;k<15;k++)
							if(k==0)
								cout << "["<<datos[i]<<"]";
							else
							cout << datos[i+k];
						cout << endl;

						break;
				}
				
				

				//4. Obtener lista de atributos
				
				//Buscar <
				while(datos[j++] != '<');
				//j++;
				
				//Concatenar cada par (atributo,valor)
				//cout << "Lista de atributos <";
				for(; datos[j] != '>'; j++){
					if( datos[j] == '(' ){
						j++; //saltar (
						for(k=0; datos[j] != ','; k++, j++){
							atributo[k] = datos[j];
						}
						atributo[k] = '\0';
						j++; //salta coma
						for(k=0; datos[j] != ')'; k++, j++){
							valor[k] = datos[j];
						}
						valor[k] = '\0';
						//cout << atributo << "," << valor <<" - ";
						n->agregarAV(atributo,valor);
					}
				}
				//cout << ">"<<endl;
				
				//5. Obtener el padre del nodo actual				
				//padre = nodos->topE();
				if(strcmp(etiqueta,"raiz") != 0)
					nodo_padre = (Nodo *) nodos->topV();
								
				//cout << "NODO: "<<etiqueta<<" VALOR: "<<tempVal<<" PADRE: "<<padre<<endl;
				
				//6. Agregar el nodo actual a la pila, y su cantidad de parentesis a la otra pila
				nodos->push((long) n, etiqueta);//puntero y etiqueta del nodo actual
				pars->push(parentesis, etiqueta); //parentesis asociados a este nodo
				
				//cout << "***** Nodo "<<etiqueta<<" con "<<parentesis<<" parentesis"<<endl;
				
				//6. *** crear el nodo nuevo en el grafo ***
				//cout << "agregar nodo en padre: "<< padre<<endl;
				//g->agregar(n, padre);
				if(strcmp(etiqueta,"raiz") != 0){
					nodo_padre->agregarSucesor(n);
					g->sumarContadorNodos();
				}
				
				i=j-1; //actualizar la posicion en datos
				break;
			case ')':
				parentesis--;
				
				if(parentesis < pars->topV()){
					//cout << "Sacando de la pila nodo: "<<nodos->topE()<<endl;
					pars->pop();  //Si se cierran los parentesis para el nodo "actual", sacarlo de la pila
					nodos->pop();
				}
				break;
			default:
				//cout << "Caracter '"<<datos[i]<<"' incorrecto, posicion: "<<i<<endl;
				break;
		}
		/*cout << "Parentesis iteracion " <<i<<endl;
		pars->listar();
		cout << "\n";
		
		cout << "Nodos iteracion " <<i<<endl;
		nodos->listar();
		cout << "\n";*/
	}
	
	return g;	
}

char * Parser::leerArchivo(char * archivo){
/*
 * Lee un archivo de texto que contiene un garfo/arbol expresado como una lista.
 * 
 * Formato:
 * 
 * Linea 1: longitud de la lista
 * Linea 2: lista
 * 
 * */	

	char * datos;
	int largo = 0;
	
	ifstream abrir;
	abrir.open(archivo);
	if(!abrir){
		cerr << "Error al abrir archivo \""<<archivo<<"\"."<<endl;
	}
	else{
		abrir >> largo;  //Leer el largo de la hilera
		cout << "Lista largo: "<<largo<<" caracteres."<<endl;
		datos = new char[largo];
		//abrir.getline(datos, largo);  //Leer esa cantidad de caracteres
		for(int i=0;i<largo;i++){
			abrir >> datos[i];
			//cout << datos[i];
		}
	}
	
	abrir.close();
	
	return datos;
}

char * Parser::obtenerRaiz(char * datos){
/*
 * Obtiene la raiz para un arbol/grafo expresado como una lista.
 * 
 * Retorna la etiqueta para el primer nodo del grafo, que equivale a la raíz.
 * 
 * */
	
	int largo = strlen(datos);
	char * temp = new char[64];
	int terminado = 0;
	int cont_temp = 0;
	
	for(int i=0;i<largo && !terminado;i++){
		if(datos[i] == '['){ //Primer nodo --> raiz
			i++; //saltar bracket
			for(cont_temp=0;i<largo && datos[i]!=']';i++, cont_temp++){
				temp[cont_temp] = datos[i];
			}
			temp[cont_temp] = '\0';
			terminado = 1;
		}
	}
	
	return temp;
}

int Parser::contarNodos(char * red){
	int total=0;
	int largo = strlen(red);
	int iterador=0;

	//Esto tiene sentido porque en la lista la etiqueta de cada nodo esta encerrada entre parentesis: [etiqueta]
	while(iterador++ < largo) if(red[iterador] == '[') total++;

	return total;
}

/*
 * Genera un arreglo doble con la mitad "izquierda" y "derecha" de un arbol, copiando la raiz
 *
 * TODO: Hay que solucionar los casos en los que la raiz tiene solo un sucesor.
 * Podria ser copiando todo hasta que haya mas de un sucesor...
 *
 * */
char ** Parser::dividir(char * lista){

	char * raiz = this->obtenerRaiz(lista);
	char ** sucesores = this->sucesoresNodo(lista, raiz);
	long cantidad_de_sucesores = (long)sucesores[0];
	long mitad = cantidad_de_sucesores/2;  //redondear hacia arriba o abajo?
	//int largo = strlen(lista);
	ostringstream subarbolI;
	ostringstream subarbolD;
	char temp[1024];
	char ** subs_temp;

	temp[0] = lista[0];
	//Recorrer todo hasta el primer parentesis
	int i=1;
	for(;i<1024 && lista[i] != '(';i++)
		temp[i] = lista[i];
	temp[i] = '\0';

	subarbolI << temp;
	subarbolD << temp;

	if(cantidad_de_sucesores == 1){
		subs_temp = dividir(subArbol(lista, sucesores[1]));
		subarbolI << "(" << subs_temp[0] << ")";
		subarbolD << "(" << subs_temp[1] << ")";
	}
	//Crear las listas para cada uno
	else{
		subarbolI << "(";
		for(i=0;i<mitad;i++){
			subarbolI << this->subArbol(lista, sucesores[i+1]);
		}
		subarbolI << "))";

		subarbolD << "(";
		for(i=mitad;i<cantidad_de_sucesores;i++){
			subarbolD << this->subArbol(lista, sucesores[i+1]);
		}
		subarbolD << "))";
	}


	int len = strlen(subarbolI.str().c_str());
	char * subarbolI_c = new char[len+1];
	strcpy(subarbolI_c, subarbolI.str().c_str());


	len = strlen(subarbolD.str().c_str());
	char * subarbolD_c = new char[len+1];
	strcpy(subarbolD_c, subarbolD.str().c_str());

	char ** subarboles = new char*[2];
	subarboles[0] = subarbolI_c;//(char *) subarbolI.str().c_str();//
	subarboles[1] = subarbolD_c;//(char *) subarbolD.str().c_str(); //

	return subarboles;
}

int Parser::cantidadSubarboles(char * datos){

	int cantidad = 0;  //this->cantSucesores(etiqueta);
	int largo = strlen(datos);
	char * temp;
	int encontrado = 0;
	int pars = 0, pars_nodo = 0;


	char * etiqueta = this->obtenerRaiz(datos);

	for(int i=0; i<largo && !encontrado; i++){
		//cout << datos[i]<<endl;
		switch(datos[i]){
			case '<':
				//saltarse toda la lista de atributos
				//cout << "saltando lista de atributos..."<<endl;
				for(; datos[i] != '>'; i++);
				break;
			case '(':
				pars++;
				break;
			case ')':
				pars--;
				break;
			case '[':
				//lo que sigue es una etiqueta, termina con ']'
				temp = new char[256];
				int j, k;
				for(j=0, k=i+1;k<largo && datos[k] != ']';k++, j++){  //k=i+1 se salta el [
					temp[j] = datos[k];
				}
				temp[j] = '\0';
				//cout << temp<<endl;
				//revisar si temp == etiqueta, y empezar aqui
				//cout << "pars para etiq \""<< temp <<"\": "<<pars<<endl;
				if(strcmp(temp,etiqueta)==0){
					//cout << "etiqueta es el objetivo... procesar resto."<<endl;
					//BUSCAR Sucesores para este nodo

					//ubicarse en el primer parentesis despues de la lista de atributos!!!
					for(;k<largo && datos[k] != '>'; k++);
					//buscar el parentesis
					for(;k<largo && datos[k] != '('; k++);
					k++; //pasar el parentesis
					pars++;  //parentesis nuevo
					pars_nodo = pars; //esta cantidad se debe mantener

					//cout << "Cantidad de parentesis para nodo \""<<temp<<"\": "<<pars<<endl;

					while(pars_nodo == pars || pars_nodo == pars+1){
						//si se abre un parentesis debe seguir un nodo
						//buscar []'s y sacar la etiqueta
						switch(datos[k]){
							case '<':
								//saltarse toda la lista de atributos
								for(; datos[k] != '>'; k++);
								break;
							case '(':
									pars_nodo++;
									//si se abre otro despues de una etiq (parsnodo == pars+2), entonces son sucesores de este otro nodo, saltar!!
									if(pars_nodo == pars+2){
										//cout << "subnodos dentro del sucesor...saltando"<<endl;
										k++; //moverse a la siguiente posicion
										while(k<largo && (pars_nodo >= pars+2)){
											switch(datos[k]){
												case '<':
													//saltarse toda la lista de atributos
													for(; datos[k] != '>'; k++);
													break;
												case '(':
														pars_nodo++;
													break;
												case ')':
														pars_nodo--;
													break;
											}
											k++;
										}
										if(datos[k] == ')') pars_nodo--; //contar el parentesis actual
										//cout << "ahora pars_nodo es: "<<pars_nodo<<endl;
										//cout << "apuntando a la posicion: " << k << "["<<datos[k]<<"]"<<endl;
									}
								break;
							case ')':
									pars_nodo--;
								break;
							case '[':
									cantidad++;
									k++; //+1 se salta el [
								break;
						}
						k++;
					}

					//cout << "inicio = "<<inicio<<", fin = "<<fin<<endl;
					encontrado = 1;
				}
				break;
		}
	}

	return cantidad;
}

int Parser::redVacia(char * datos){
	int i=1;
	int vacia = 1;
	while(datos[i] != '(') i++;

	for(;i<strlen(datos) && vacia;i++)
		if(datos[i] == '[') vacia = 0;

	return vacia;
}
