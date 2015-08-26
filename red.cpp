#include "red.h"
#include <ctime>
#include <iostream>
#include <sstream>
#include <cstdlib>

using namespace std;

Red::Red(){
	//grafo = new Grafo(new Norma("raiz"));
	raiz = new Norma((char *) "raiz");
	this->nodo_actual = raiz;

	sim_viajes = new Similitud();

	cg = new CargarCasos();

	this->cantidad_total_de_nodos = 1;
	this->nodinho = 0;
}

Red::~Red(){
	if(raiz)
		//BorrarRed(raiz);
		delete raiz;
}

void Red::BorrarRed(Nodo * N){
	int cant_sucs = N->cantSucesores();
	for(int i=0;i<cant_sucs;i++){
	  //Borrado recursivo
		BorrarRed(N->getSucesor(i));
	}
	delete N;
	//cout << "Se va a borrar el nodo "<< N->id_nodo()<<endl;
}

int Red::generar(char * archivo){
	if(cg->leerArchivo(archivo) == -1){
		return -1;
	}
	int cantidad_de_casos = cg->cantidadCasos(); //1470; //*** Buscar una forma de calcular la cantidad de casos en forma automatica ***
	cantidad_total_de_casos = cantidad_de_casos;
	base_de_casos = cg->getCasosM();

	this->nombre_columnas = cg->nombreColumnas();
	this->cantidad_de_columnas = cg->cantidadColumnas();

	//Establecer la semilla con el tiempo para la generacion de numeros aleatorios
	t = time(NULL);
	srand(t);

	//Crear archivo para histograma de crecimiento
	//salida.open("etiquetas.dat",ios::out);

	this->generarR(base_de_casos, cantidad_de_casos, 0);

//	salida.close();

	//cout << "Total nodos: "<<this->contar_nodos()<<endl;
	//cout << "Total casos: "<<nodinho<<endl;
}

void Red::generarR(char *** casos, int cantidad_de_casos, Norma * _norma){
	int cant_dim_1 = 0;  //cantidad de dimensiones
	int cant_sucs = 0;  //cantidad de sucesores del nodo actual, para los ciclos
	char ** parAV;  //Contiene el par atributo valor actual
	char ** c;  //caso actual, vector de hileras
	//lista_caso c;  //contiene el caso leido actualmente
	Norma * N = 0;  //Norma destino
	Nodo * nodo;  //Nodo temporal, usualmente un INDICE, para agregar los casos
	//Estos dos serán usados para agregar nodos al grafo
	Indice * I;  //Para agregar indices
	Caso * caso; //Para agregar casos
	
	char id[24];

	int nodo_encontrado = 0;
	ListaAV * dim; //Conjunto de indices del caso (dimensiones)
	

	for(int i=1; i<=cantidad_de_casos; i++){

		//Para cada caso en la colección...
		//cout << "Leyendo caso " <<i<<" en la sub-coleccion."<<endl;
		c = casos[i];
		//cout << "Procesando caso ID="<<c[0]<<endl;
		//cout << "Procesando caso = " << c[0]<<endl;
		if(_norma){
			//cout << "Se hizo llamada recursiva a generarR, con la norma "<< _norma->id_nodo()<<endl;
			//cout << "caso = " << c[0]<<endl;
			N = _norma;
		}
		else
			N = buscarNorma(c);

		//cout << "Seleccionada norma: " << N->id_nodo() << endl;
		//if(strcmp(N->id_nodo(),raiz->id_nodo()) != 0) cout << "Norma "<<N->id_nodo()<<" seleccionada!."<<endl;

		dim = calcularDimensiones(c,N->getListaAV());

		//dim->verContenido();
		cant_dim_1 = dim->longitud();
		
		//cout << cant_dim_1 << " dimensiones" <<endl;
		
		//Para cada dimension (a,v) del caso...
		for(int j=0; j<cant_dim_1; j++){
			parAV = dim->getParAV(j);
			//Buscar sucesor en la norma N, con atributo a
			
			/**
			 * POR AQUI ESTA EL ERROR DE CREACION
			 * **/

			//cout << "Procesando atributo: " << parAV[0] <<endl;
			
			cant_sucs = N->cantSucesores();
			nodo_encontrado = 0;

			//cout << "Cantidad de sucesores de la norma "<<N->id_nodo()<<": "<<cant_sucs<<endl;

			for(int k=0; k<cant_sucs && !nodo_encontrado; k++){
				//cout << "Sucesor "<<k+1<<"("<<N->getSucesor(k)->id_nodo()<<") es un INDICE ="<<(N->getSucesor(k)->isa() == INDICE)<<endl;

				/*
				 * Aqui hay un problema cuando se usan muchos casos.  Por alguna razon se "pierde" el puntero al sucesor de la Norma
				 * y al utilizar sus funciones produce un "segmentation fault"
				 * ARREGLADO: punteros de 64-bit
				 * */

				nodo = N->getSucesor(k);  //k-esimo sucesor de la norma elegida
				//ver si tiene el atributo
				//Se supone que todos los sucesores de esta norma son INDICES

				if(nodo->isa() == INDICE){
					if(strcmp(parAV[0], ((Indice *)nodo)->getAtributo()) == 0){
						//Si tiene la misma etiqueta (atributo)
						nodo_encontrado = 1;
					}
				}
			}
			
			//"nodo" es el Indice donde debe indizarse si nodo_encontrado es V
			if(nodo_encontrado){
				//Comprobar que no tenga el valor en parAV actual
				for(int k=0;k<nodo->cantParesAV() && nodo_encontrado == 1;k++){
					if(strcmp(parAV[1], ((Indice *)nodo)->getValor(k)) == 0){
						nodo_encontrado = 0;
						//Existe el indice con el valor actual.  Ya hay un nodo indizado.
						//Si nodo->getSucesor(k)->isa() == CASO, entonces
						if(nodo->getSucesor(k)->isa() == CASO){
							//cout << "Se debe crear una norma para el par ("<<parAV[0] << "," <<parAV[1]<<") - caso "<< c[0] <<endl;

							/****************************************************/
							/*       CODIGO PARA CREACION DE NORMAS (P3)        */
							/****************************************************/

							/*
							 * 1. Crear una norma en el lugar del caso
							 * 	1.1 Obtener el caso de la lista de sucesores
							 * 	1.2 Borrar el caso de la lista de sucesores
							 * 	1.3 Borrar el atributo en el índice
							 * 	1.4	Crear un par (a,v) nuevo y una norma nueva y agregar ambos al indice.
							 * 2. Extraer similitudes entre los dos casos, y agregarlas como características compartidas en la norma.
							 * 	2.1 Aplicar un algoritmo de similitudes que retorne una listaAV.  PERO: ambos casos comparten solo UNA dimension. **Más todas las "anteriores"
							 * 	2.2 Agregar esos pares (a,v) a la norma creada
							 * 3. Indizar los dos casos en la norma creada
							 * 	3.1 Hacer N = esa norma
							 * 	3.2 calcularIndices() excluyendo los anteriores
							 *
							 *
							 */
							//1.1
							//cout << "Paso 1.1"<<endl;
							//Caso * caso_temp = new Caso(nodo->getSucesor(k)->id_nodo());

							/*** Solo se necesita el JourneyCode del caso, lo demas se saca de la base de casos***/
							char * id_caso = nodo->getSucesor(k)->getParAV(0)[1];
							//***caso_temp->setAV(nodo->getSucesor(k)->getParAV(0)[0],nodo->getSucesor(k)->getParAV(0)[1]);***/

							//caso_temp->setListaAV(nodo->getSucesor(k)->getListaAV());

							//1.2
							//cout << "Paso 1.2"<<endl;
							nodo->eliminarSucesor(k); //1.3 incluido: se borra el par (a,v) al borrar el sucesor
							this->cantidad_total_de_nodos--;
							nodinho--;
							//1.4
							//cout << "Paso 1.4"<<endl;
							//cout << "Generando id"<<endl;
							this->generarID('N',parAV,i,j, id);
							//cout << "Creando norma"<<endl;
							Norma * norma_temp = new Norma(id);
							//cout << "Agregando parAV"<<endl;
							nodo->agregarAV(parAV[0],parAV[1]);
							//this->verContenido();
							//cout << "Agregando como sucesor del nodo "<<nodo->id_nodo()<<endl;
							nodo->agregarSucesor(norma_temp);
							this->cantidad_total_de_nodos++;


							//Norma creada y agregada en el lugar del caso, indizada por el par (a,v) del caso actual (mismos que el caso preexistente)

							//2.1 y 2.2 Agregar pares compartidos a la norma
							//cout << "Paso 2.1 y 2.2"<<endl;
							//ListaAV * compartidas = new ListaAV();

							/* Es innecesario porque ahora todo esta en el nodo, o al menos para c actual */
							int pos_caso_1=this->posicionBaseDeCasos(id_caso);
							//int pos_caso_2=this->posicionBaseDeCasos(c[0]);

							//cout << "posiciones de los casos: " << pos_caso_1 <<" y "<<pos_caso_2<<endl;

							//cout << "la posicion del caso "<<id_caso<<" en la base es: "<<pos_caso<<endl;
							ListaAV * dimensiones_caso_1 = this->calcularDimensiones(base_de_casos[pos_caso_1]);
							ListaAV * dimensiones_caso_2 = this->calcularDimensiones(c);//base_de_casos[pos_caso_2]);

							//cout << "id_caso_temp = " <<id_caso_temp<<endl;
							char ** parAV_actual;

							for(int p=0;p<dimensiones_caso_1->longitud();p++){
								//dim es la listaAV del par que esta siendo agregado, dimensiones_caso es la listaAV del caso que ya existia
								parAV_actual = dimensiones_caso_1->getParAV(p);
								//cout << "Comparando el par (" << parAV_actual[0] << ", " << parAV_actual[1]<< ")"<<endl;

								if(dimensiones_caso_2->existeAtributoValor(parAV_actual)){
									//cout << "Par existe en dim"<<endl;
									//cout << "Comparten el par (" << parAV_actual[0] << ", " << parAV_actual[1]<< ")"<<endl;
									norma_temp->agregarAV(parAV_actual[0], parAV_actual[1]);
								}
							}

							//cout <<this->generarLista()<<endl;


							//Crear una "mini coleccion" de casos usando solo los dos en cuestion, c y base_de_casos[id_caso_temp]

							char *** base_de_casos_tmp = new char**[3];

							base_de_casos_tmp[0] = base_de_casos[0]; //Encabezados
							base_de_casos_tmp[1] = base_de_casos[pos_caso_1];
							base_de_casos_tmp[2] = c;//base_de_casos[pos_caso_2];

							/*cout << "**************** Contenido de la base de casos temp **************"<<endl;

							for(int q=0;q<2;q++){
								cout << "Caso "<<q+1<<endl;
								for(int w=0;w<9;w++){
									cout << base_de_casos_tmp[q][w]<<endl;
								}
								cout << "-----------"<<endl;
							}*/

							//3,1 y 3,2
							//cout << "Paso 3.1 y 3.2"<<endl;

							/*********************
							 *
							 this->verContenido();
							 cin.get();
							 *
							 **********************/

							//cout << "Generando nueva norma "<< norma_temp->id_nodo() <<" para los casos "<< base_de_casos_tmp[1][0] << " y "<<base_de_casos_tmp[2][0] << endl;
							this->generarR(base_de_casos_tmp, 2, norma_temp);
							/*
								Ahora deberia limpiar todas las variables creadas...
							*/
							delete [] base_de_casos_tmp;

							//cout << "continuando con el resto de los casos..."<<endl;

						}
						else{
							//Si nodo->getSucesor(k)->isa() == NORMA.  Si existe una norma donde debe indizarse un Caso (P4)
							if(nodo->getSucesor(k)->isa() == NORMA){
								//cout << "Se debe refinar la norma para el par ("<<parAV[0] << "," <<parAV[1]<<") en el indice "<<nodo->getSucesor(k)->verParesAV()<<endl;

							/****************************************************/
							/*     CODIGO PARA REFINAMIENTO DE NORMAS (P4)      */
							/****************************************************/
								//Indizar el caso en esa norma

								//1. Extraer los atributos del caso
								int pos_caso = this->posicionBaseDeCasos(c[0]);
								char *** base_de_casos_tmp = new char**[2];

								base_de_casos_tmp[0] = base_de_casos[0]; //Encabezados
								base_de_casos_tmp[1] = base_de_casos[pos_caso];

								//2. Llamada recursiva con el caso actual en la norma seleccionada (nodo->getSucesor(k))
								this->generarR(base_de_casos_tmp, 1, (Norma *) nodo->getSucesor(k));

								delete [] base_de_casos_tmp;

								//3. Validar y actualizar caracteristicas compartidas en la Norma (listaAV en nodo)
								/****************************/
								/** ¿Es necesario? **/
								/*
								 * 1. Buscar un par atributo-valor no compartido
								 * 	1.1 Recorrer la listaAV de la norma y compararla con todos sus indices
								 * 2. Crear un indice con ese atributo
								 * 3. Crear una subnorma vacia
								 * 4. Indizarla con ese valor
								 * 5. Reunir todos los otros pares no compartidos y agregarlos a ListaAV de la subnorma
								 * 6. Marcar la subnorma como generalizada
								 * */

								/*
								 * HAY QUE COMPROBAR LOS PARES DE LOS INDICES O DE LOS CASOS???
								 *
								 * */
/*
							/**********************************************************/
							/*     ACTUALIZACION DE NORMAS (P5) - POR AHORA NO HACER  */
							/**********************************************************/
 /*
								nodo = nodo->getSucesor(k);  //La norma por validar
								cout << "Comprobando si se debe generalizar la norma: "<<nodo->id_nodo() << " "<<nodo->verParesAV()<<endl;

								//Crear la norma nueva
								this->generarID('N',parAV, i,j, id);
								Norma * norma_nueva = new Norma(id);

								ListaAV * lista_AV_norma = nodo->getListaAV();
								ListaAV * av_nueva_norma = norma_nueva->getListaAV();
								char ** par_av;
								char ** par_av_I;
								Nodo * indice_norma;
								int n_sucs = nodo->cantSucesores();
								int atr_compartido = 1;

								//CREAR LA LISTA_AV DE ATRIBUTOS NO COMPARTIDOS:
								//Recorrer la lista de atributos-valores
								cout << "calculando pares no compartidos"<<endl;

								cout << "Indices de la norma actual: "<<endl;
								for(int cont_ind=0;cont_ind<n_sucs;cont_ind++){
									indice_norma = nodo->getSucesor(cont_ind);
									cout << "INDICE "<<cont_ind+1<<" :"<<indice_norma->verParesAV()<<endl;
								}

								for(int pav=0;pav<lista_AV_norma->longitud();pav++){
									par_av = lista_AV_norma->getParAV(pav);
									atr_compartido = 1;
									//Por cada par (a,v) compararlo con los pares (a,v) de cada indice
									for(int cont_ind=0;cont_ind<n_sucs && atr_compartido;cont_ind++){
										indice_norma = nodo->getSucesor(cont_ind);
										//Determinar si par_av NO existe en la listaAV de algun indice => no es compartido
										if(!indice_norma->getListaAV()->existeAtributoValor(par_av)){
											//Si no existe sacar de la norma y pasarlos a una nueva listaAV
											cout << "NO existe par (" << par_av[0] << ", "<< par_av[1]<<")"<<endl;
											av_nueva_norma->agregar(par_av[0], par_av[1], NORMA);
											atr_compartido = 0;
										}
									}
								}

								cout << "total pares no compartidos = "<<av_nueva_norma->longitud()<<endl;

								if(av_nueva_norma->longitud() > 0){

									//Eliminar todos los pares no compartidos de la norma ("nodo")
									int pos_par_av;
									for(int pav=0;pav<av_nueva_norma->longitud();pav++){
										pos_par_av = lista_AV_norma->existeAtributoValor(av_nueva_norma->getParAV(pav));
										if(pos_par_av > -1)
											lista_AV_norma->eliminar(pos_par_av);
									}

									//Crear un indice nuevo y una norma vacia
									par_av = av_nueva_norma->getParAV(0);  //utilizar el primer par (a,v) para indizar

									this->generarID('I',par_av, i,j, id);
									Indice * indice_nuevo = new Indice(id);
									//Marcar la norma como generalizada
									norma_nueva->marcarGeneralizada();

									//Agregar el indice como sucesor de la norma original
									nodo->agregarSucesor(indice_nuevo);
									this->cantidad_total_de_nodos++;
									//Indizar la norma usando par_av
									indice_nuevo->agregarAV(par_av[0],par_av[1]);
									indice_nuevo->agregarSucesor(norma_nueva);
									this->cantidad_total_de_nodos++;

									//Limpiar las variables
									delete lista_AV_norma;
									delete par_av;
									delete par_av_I;
									delete indice_norma;
								}
								else{
									delete norma_nueva;
								}
								*/

							}
						}
					}
				}
				if(nodo_encontrado){
					//Existe el INDICE pero no el valor para el caso actual.
					//Agregar solo el nuevo valor y el caso como sucesor del indice actual.

					//cout << "Agregando valor " << parAV[1] << " al indice preexistente " << nodo->id_nodo() << endl;

					nodo->agregarAV(parAV[0],parAV[1]);
					//id = new char[16];
					this->generarID('C',parAV,i,j, id);
					nodinho++;
					caso = new Caso(id);
					//caso->agregarAV(c[0],c[0]);

					// MODIFICADO PARA QUE LOS CASOS CONTENGAN TODOS LOS PARES A,V
					caso->agregarListaAV(nombre_columnas, c, cantidad_de_columnas);

					nodo->agregarSucesor(caso);
					this->cantidad_total_de_nodos++;

					//cout << "Similitud indice con caso: "<<this->similitud(nodo, c)<<endl;
					//Se ha terminado de procesar por completo un caso
					//Generar histograma de crecimiento
					//salida << this->tamano()<<endl;
				}
			}
			//Si no se encontro el nodo (no existe nodo con atributo buscado):
			else{
				//cout << "Creando INDICE y CASO nuevos para el caso "<<c[0] << " en la norma "<<N->id_nodo()<<endl;
				this->generarID('I',parAV,i,j,id);
				I = new Indice(id);
				I->agregarAV(parAV[0], parAV[1]);
				//cout << "*** puntero I: "<<I<<" INT: "<<(int)I<<" *** "<<endl;

				this->generarID('C',parAV,i,j, id);
				nodinho++;
				caso = new Caso(id);
				//caso->agregarAV(c[0],c[0]);
				// MODIFICADO PARA QUE LOS CASOS CONTENGAN TODOS LOS PARES A,V
				caso->agregarListaAV(nombre_columnas, c, cantidad_de_columnas);
				
				I->agregarSucesor(caso);
				this->cantidad_total_de_nodos++;
				//cout << "Agregado sucesor " << caso->id_nodo() << " al indice " << I->id_nodo() << endl;
				N->agregarSucesor(I);
				this->cantidad_total_de_nodos++;
				//cout << "Agregado sucesor " << I->id_nodo() << " a la norma " << N->id_nodo() << endl;
				
				//cout << "Se ha indizado por ("<<parAV[0]<<", "<<parAV[1]<<")"<<endl;

				//Se ha terminado de procesar por completo un caso
				//Generar histograma de crecimiento
				//salida << this->tamano()<<endl;

			}
		}
		
	}
}

int Red::posicionBaseDeCasos(char * id_caso){
	//buscar el caso en la base (barrido lineal...podria hacerse mejor?)
	int caso_encontrado = 0;
	int pos_caso=1;
	while(pos_caso<cantidad_total_de_casos && !caso_encontrado){
		//cout << "comparando "<<base_de_casos[pos_caso][0]<<" con "<<id_caso<<": ";
		if(strcmp(base_de_casos[pos_caso][0], id_caso) == 0){
			//cout << base_de_casos[pos_caso][0]<<" = "<<id_caso<<endl;
			caso_encontrado = 1;
		}
		else{
			pos_caso++;
		}
	}
	return pos_caso;
}

void Red::probar(){
	cg = new CargarCasos();
	cg->leerArchivo((char *) "coleccion.csv");
	char *** casos = cg->getCasosM();
	
	char ** c = casos[0];
	
	cout << "Caso: ";
	
	for(int i=0;i<10;i++){
		cout << c[i] << ", ";
	}
	
	cout << "Sacar nombres de dimensiones\n";
	
	ListaAV * l = new ListaAV();

	ListaAV * dim = this->calcularDimensiones(c, l);
	
	cout << "listo"<<endl;
	
	cout << dim->longitud() << " Dimensiones del caso: "<< dim->listarPares() << endl;

	cout << "sacando parAV" <<endl;		

	char ** parAV = dim->getParAV(1);
	
	cout << "parAV = (" << parAV[0] << "," << parAV[1] << ")"<<endl; 
	
    char * id2[12];

	for(int i=0;i<5;i++){
		parAV = dim->getParAV(i);
		id2[i] = new char[20];
		generarID('I',parAV, 1,i,id2[i]);
		//cout << id2[i] << endl;
	}

	cout << "IDs: "<<endl;
	for(int i=0;i<5;i++){
		cout <<	id2[i] <<endl;
	}

	//this->generarID('I',parAV,id);
	
	//cout << "Etiqueta 1: " << id <<endl;
	
	//this->generarID('C',parAV, id);
	
	//cout << "Etiqueta 2: " << id <<endl;
}

/*
 * Funcion para generar etiquetas de identificación únicas dentro de la red.
 * Utiliza el tipo de nodo (N,I,C), los primeros 2 caracteres del atributo, los primeros 2 del valor, el numero de caso, el numero de indice, y los últimos 5 dígitos de la hora en formato Unix.
 * Ej: Si tipo = N, parAV = [Atributo,Valor], val1 = 10, val2 = 7 y la fecha = 12345678890 => id = NAtVa107_67890
 * */
void Red::generarID(char tipo, char ** parAV, int val1, int val2, char *id){
//	char id[20]; //1 char tipo, 3 chars de Atributo, 3 chars de Valor, 5 chars del tiempo, 1 fin de linea '\0'

	//Si se asume que cada llamda a generarID es para construir un nodo...
	//this->cantidad_total_de_nodos++; Excepcion: Al agregar norma se cuenta 2 veces
	
	char v1[12];
	char v2[12];
	char t_str[12];

	//cout << "Generando id con tiempo "<<t<<endl;	
	
	int i=0;
	
	id[i++] = tipo;
	
	id[i++] = parAV[0][0];
	id[i++] = parAV[0][1];
	
	unsigned int j=0;
	while(j < strlen(parAV[1]) && j < 2) id[i++] = parAV[1][j++];

	this->itoa(val1,v1);
	this->itoa(val2,v2);

	int k=0;
	while(v1[k] != '\0') id[i++] = v1[k++];
	k=0;
	while(v2[k] != '\0') id[i++] = v2[k++];
	
	id[i++] = '_';


	//*** OPCION 1: Agregar los ultimos 5 digitos de la hora actual (en segundos)
	time_t t2 = time(NULL);
	this->itoa(t2,t_str);
	k=7; //strlen(t_str)/2; //USAR 3 DIGITOS
	while(t_str[k] != '\0') id[i++] = t_str[k++];
	
	//NUEVO: agregar un numero aleatorio de 5 digitos:
	char num[6];
	int aleat = rand()%89999 + 10000;
	this->itoa(aleat, num);
	k = 0;
	while(k < 5) id[i++] = num[k++];

	//*** OPCION 2: Agregar la cantidad actual de nodos
/*
	char num[8];
	this->itoa(this->nodinho++, num);
	k = 0;
	while(k < strlen(num)) id[i++] = num[k++];
*/

	id[i] = '\0';
	
	//salida << id <<endl;
}

int Red::tamano(){
	return this->cantidad_total_de_nodos;
}

//Busca la norma donde debe indizarse el caso 'c'.  Podria retornar solo su ID, no el puntero.
Norma * Red::buscarNorma(char ** c){
	Norma * N;
	
	if(raiz->cantSucesores() > 0){
		//Norma raiz vacía
		/*
		 * Aplicar algoritmo de busqueda y similitud, elegir la norma cuyos pares (a,v) son mas semejantes al caso c
		 * CRITERIO DE SIMILITUD
		 *
		 * */
		N = raiz; //buscarNormaR(raiz, c, (double) 0);  //sim >= 0
		//N = raiz;
	}
	else{
		N = raiz;
	}
	
	return N;
}

Norma * Red::buscarNormaR(Nodo * N, char ** c, double sim){
	Norma * respuesta = raiz;
	int cant_sucs = 0;
	double max_sim=0;
	double sim2=0;
	int pos=0;
	//buscar las normas sucesoras de esta, habria que comparar con los indices?
	if(this->similitud(N,c) >= sim && N->isa() == NORMA){ //evaluar usando criterio similitud
		//Si es una norma y supera el criterio
		respuesta = (Norma *) N;
	}
	else{
		cant_sucs = N->cantSucesores();
		for(int i=0;i<cant_sucs;i++){
			//Sino buscar el sucesor con mayor similitud que no sea CASO
			if(N->getSucesor(i)->isa() != CASO){
				sim2 = similitud(N->getSucesor(i),c);
				if(sim2 > max_sim){
					max_sim = sim2;
					pos = i;
				}
			}
		}
		//Necesariamente hay un nodo "mejor" que los otros, continuar la busqueda con ese
		respuesta = buscarNormaR(N->getSucesor(pos), c, sim);
	}

	return respuesta;
}

/*
 * Usar Hamming o Tversky
 * Nodo 'n' puede ser una norma o un índice
 * 'c' es un caso o un problema
 * El caso (o problema) contiene atributos, que podrían no estar presentes en 'n'
 * El indice contiene atributos "repetidos" con multiples valores
 * */
double Red::similitud(Nodo * n, char ** c){
	double sim = 0.0;

	/*
	 *
	sim_t = alfa*f(A) - beta*f(B) - gama*f(C) - delta*f(D)
	A = x_i === y_i
	B = x_i && y_i con valores diferentes PERO SEMEJANTES
	C = x_i && ¬y_i
	D = ¬x_i && y_i --¿Es necesario? por ahora es en realidad la cantidad de pares que coinciden solo en atributo

	A, B, C conjuntos
	*/

	//Obtener los pares atributo-valor de n
	ListaAV * listaNodo;
	//AJUSTE: Ahora los casos tienen toda la lista de pares a,v
	if(n->isa() == CASO){
		listaNodo = n->getListaAV(); //Eliminar el primer par, JourneyCode.  No importa porque no estara presente en c
//		listaNodo->eliminar(0);
	}
	//this->calcularDimensiones(base_de_casos[this->posicionBaseDeCasos(n->getParAV(0)[0])]);
	else{
		listaNodo = n->getListaAV(); //nota: si es un indice, tiene atributos repetidos
	}

	ListaAV * listaCaso = this->calcularDimensiones(c);

	int total = listaCaso->longitud();  //Para normalizar: num pares en el caso
	int totalN = listaNodo->longitud(); //Num pares en el otro nodo.

	//Generar conjuntos: A - pares iguales en n y c, B - pares existentes en n y c con diferente valor, C - pares en c y no en n
	//MOD: D - pares en n y no en c

	// NUEVA DEFINICION: A E (a,v), B E (a,~v), C E (a,~v), D ¬E a
	int A = 0, B = 0, C = 0, D = 0;

	char ** parX;
	int pos=-1;
	short valorEncontrado = 0;

	for(int i=0;i<total;i++){

		parX = listaCaso->getParAV(i);  //par A,V actual (del caso)
		pos = listaNodo->existeAtributo(parX[0]);

		if(pos >= 0){
			//Dos casos: los valores coinciden o no
			//Recorrer toda la lista de atributos del nodo para comparar.
			//Esto porque si es un índice, existe el mismo atributo varias veces con diferentes valores.
			valorEncontrado = 0;
			for(int j=pos;j<totalN && !valorEncontrado; j++){
				if(strcmp(listaNodo->getParAV(j)[1],parX[1]) == 0){  //Si el par (a,v) es exactamente igual
					//cout << "Comparten el par ("<<parX[0]<<","<<parX[1]<<")"<<endl;
					valorEncontrado = 1;
					A++;
				}
				else if(sim_viajes->sim(listaNodo->getParAV(j)[1],parX[1], parX[0], cg->getTipo(parX[0]))){  //Si el par (a,v) esta en el rango considearado "igual"
					valorEncontrado = 1;
					B++;
				}
			}
			if(!valorEncontrado){ //No existe el valor para ese atributo
				//cout << "Comparten solo el atributo \""<<parX[0]<<"\""<<endl;

				//Pero si es un indice tiene múltiples valores para ese atributo... hay que comparar con todos?? Que pasa si varios estan en el rango???
				C++; //+= sim_viajes->sim(listaNodo->getParAV(pos)[1],parX[1], parX[0], cg->getTipo(parX[0]));
			}
		}
		else{
			D++;
		}
	}

	//además sumar los atributos presentes en el nodo y NO en el caso
	switch(n->isa()){
		case NORMA:
		case CASO:
			for(int i=0;i<totalN;i++){
				if(!listaCaso->existeAtributo(listaNodo->getParAV(i)[0])) D++;
			}
			break;
		case INDICE:
			if(!listaCaso->existeAtributo(((Indice *) n)->getAtributo())) D++;
			break;
	}

	//D = 0;//algo....

	//Calcular sim_t
	/*
	 * A, B, C son "acumuladores" o bien f es la función suma/cardinalidad.  Entonces f(A) es la cantidad de apariciones de x tal que x pertenece A, o bien la cantidad de elementos que cumplen A.
	 *
	 * alfa, beta y gama son los pesos/relevancia.
	 *
	 * La similitud de Tversky es:
	 *
	 * sim_t = alfa*A - beta*B - gama*C
	 *
	 * */
	double alfa = 1.0;
	double beta = 0.5;
	double gama = 0.1;
	double delta = 0.1;

	//cout << "Similitud para este caso: A = " << A << ", B = " << B << " C = " << C <<endl;

	sim = (alfa*A + beta*B - gama*C - delta*D) / total;

	if(sim<0) sim*= -1;

	return sim;
}

//definir funcion

/*
 * Genera una lista de pares (a,v) para el caso actual.
 * Utilizado para indizar casos en normas recién creadas, excluyendo aquellas dimensiones contenidas en la norma
 *
 * */
ListaAV * Red::calcularDimensiones(char ** c, ListaAV * lista){
	ListaAV * l = new ListaAV();
	char ** cols = cg->nombreColumnas();
	
	//Iniciar en 1 para deshacerse del JourneyCode (no PREDICTIVO)
	for(int i=1;i<cg->cantidadColumnas();i++){
		if(lista->existeAtributo(cols[i]) == -1){
			//Agregar solo aquellos atributos que no estan en la lista
			//Esto equivale a deshacerse de las dimensiones NORMATIVAS, pues Lista contiene los atributos de la NORMA elegida
			//para el caso actual.
			l->agregar(cols[i],c[i],CASO);
		}
	}

	return l;
}

/*
 * Solo calcular las dimensiones
 * */
ListaAV * Red::calcularDimensiones(char ** c){
	ListaAV * l = new ListaAV();
	//char ** cols = cg->nombreColumnas();

	//Iniciar en 1 para deshacerse del JourneyCode (no PREDICTIVO)
	for(int i=1;i<this->cantidad_de_columnas;i++){
		if(strcmp(c[i],"")!=0)  //Agregar solo cuando tiene valores
			l->agregar(this->nombre_columnas[i],c[i],CASO);
	}

	return l;
}

void Red::verContenido(){

}

/* 	COMENTAR POR AHORA, NO SON NECESARIAS
void Red::verContenido(char * archivo){
	fstream salida;
	salida.open(archivo,ios::out);

	if(!raiz)
		cout << "Red vacia\n";
	else
		verContenidoR(raiz, 1, &salida);

	salida.close();
}


void Red::verContenidoR(Nodo * nodo, int nivel, fstream * salida){
	//if(nodo->isa() == CASO) cout << "\t";

	//cout << "Nodo con " << sucs << " sucesores | ";
	//if(strcmp(nodo->id_nodo(),"raiz") == 0) cout << "Raiz: "<<nodo->verParesAV()<<endl;
if(nodo->isa() == NORMA){
	if(salida)
		*salida << nodo->isa_txt() << "["<< nodo->id_nodo() <<"]"<< nodo->verParesAV() << endl;
		//*salida <<"["<< nodo->isa_txt()<<"]" << nodo->verParesAV() << endl;
	else
		cout << nodo->isa_txt() << " ["<< nodo->id_nodo() <<"] "<< nodo->verParesAV() << endl;
}

	if(nodo->isa() != CASO){
		int sucs = 0;
		sucs = nodo->cantSucesores();

		for(int i=0;i<sucs;i++){
			for(int j=0;j<nivel;j++){
				if(salida)
					*salida << "\t";
				else
					cout << "   ";
			}
			verContenidoR(nodo->getSucesor(i), nivel+1, salida);
		}
	}
}*/

/*
 * Funciones 'itoa' y 'reverse' tomadas de:
 * Kernighan & Ritchie: "The C Programming Language".  2º Edicion, (1988). pp. 64
 *
 * */
/* reverse:  reverse string s in place */
void Red::reverse(char s[])
{
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/*
 * Funciones 'itoa' y 'reverse' tomadas de:
 * Kernighan & Ritchie: "The C Programming Language".  2º Edicion, (1988). pp. 64
 *
 * */
/* itoa:  convert n to characters in s */
void Red::itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}

int Red::contar_nodos(){
	int total = contar_nodos_R((Nodo *) raiz);

	return total;
}

int Red::contar_nodos_R(Nodo * nodo){
	int cant_sucs = nodo->cantSucesores();

	int suma = 1; //Contarse a si mismo

	//cout << nodo->isa_txt() << " ["<< nodo->id_nodo() <<"] "<< nodo->verParesAV();

	for(int i=0;i<cant_sucs;i++) suma += contar_nodos_R(nodo->getSucesor(i));

	return suma;
}

/*
 * CONSIDERAR LA POSIBILIDAD DE GENERAR LISTAS A PARTIR DE CUALQUIER NODO, LO CUAL ES TRIVIAL (CAMBIAR EL NODO INICIAL)
 * EL PROBLEMA ES COMO HACER LA REFERENCIA DESDE AFUERA
 *
 * */
char * Red::generarLista(){
	//char * lista;
	ostringstream lista;

	//ostringstream * oss = &lista;

	//*oss << "Inicio"<<endl;

	//cout << lista.str();

	//nodinho = 1;

	nodinho = 0;

	if(raiz){
		//cout << "Declarando lista de ~ 1G caracteres"<<endl;
		//lista = new char[1000000000];
		//cout << "Generando lista..."<<endl;
    	this->listaR(raiz, &lista);
	}
 	else{
 		lista << "()";
    	//lista = (char *) "()";
 	}


	int len = strlen(lista.str().c_str());

	//cout << "lista creada tam: "<<len<<endl;

	//cout << "total casos: "<<nodinho<<endl;

	char * lista_c = new char[len+1];

	strcpy(lista_c, lista.str().c_str());

	//cout << "lista_c"<<lista_c<<endl;

	return lista_c; //(char *) lista.str().c_str();
}

/*
 * Listar Recursivo, utilizado por generarLista para recorrer la red en forma recursiva.
 * */
void Red::listaR(Nodo * nodo, ostringstream * lista){


    //si es caso
    if(nodo->isa() == CASO){
    	//cout << nodinho++ <<endl;
      //cout << "("<<nodo->isa_txt() << nodo->verParesAV() << ")";
      //sprintf(lista, "%s(%c[%s]%s)", lista, nodo->isa_txt(),nodo->id_nodo(),nodo->verParesAV());
      *lista << "("<<nodo->isa_txt() << "["<<nodo->id_nodo()<<"]"<<nodo->verParesAV() << ")";
      nodinho++;
    }
    else{
		//cout << "("<<nodo->isa_txt() << nodo->verParesAV() << "(";
		//sprintf(lista, "%s(%c[%s]%s (",lista, nodo->isa_txt(),nodo->id_nodo(),nodo->verParesAV());
    	//cout << nodinho++ <<endl;
		*lista << "("<<nodo->isa_txt() << "["<<nodo->id_nodo()<<"]"<<nodo->verParesAV()<< "(";

		int cant_sucs = nodo->cantSucesores();

		for(int i=0; i<cant_sucs; i++){
			this->listaR(nodo->getSucesor(i), lista);
		}
		//cout << "))";
		//sprintf(lista, "%s))", lista);
		*lista << "))";
    }
}

/*
 * Función para agregar "nodos" cuando se reconstruye la red a partir de una lista de texto previamente generada.
 * */
 
int Red::agregar(Nodo * nodo, char * id){
	int exito = 0;
	if(strcmp(nodo->id_nodo(),"raiz")==0){
		//NADA, ya hay raiz.
		/*raiz = (Norma *)nodo;
		this->cantidad_total_de_nodos++;
		exito = 1;*/
		//cout << "agregada raiz: "<<id<<endl;
	}
	else{ //buscar nodo con id
		exito = agregarR(raiz, id, nodo); 
	}
	return exito;
}

/*
 * Este metodo se debe OPTIMIZAR, ya que hace recorrido semi-exhaustivo (con meta).
 * */
int Red::agregarR(Nodo * padre, char * id, Nodo * nodo){
	int sucs = 0;
	int encontrado = 0;

	if(strcmp(padre->id_nodo(),id) == 0){
		padre->agregarSucesor(nodo);
		encontrado = 1;
		this->cantidad_total_de_nodos++;
		cout << "agregado nodo: "<<nodo->id_nodo() << " a nodo "<<id<<endl;
	}else{
		sucs = padre->cantSucesores();
		//cout << "expandiendo "<<sucs<<" sucesores en nodo "<<padre->id_nodo()<<endl;
		for(int i=0;i<sucs && !encontrado;i++){
			encontrado = agregarR(padre->getSucesor(i), id, nodo);
		}
	}

	return encontrado;
}
 
Nodo * Red::getRaiz(){
	return (Nodo *) raiz;
}

void Red::sumarContadorNodos(){
	this->cantidad_total_de_nodos++;
}


 /*************************************
 *
 * Funciones de recuperacion de casos
 *
 * ************************************/

Nodo * Red::nodoActual(){
	return this->nodo_actual;
}

void Red::nodoSiguiente(Nodo * nodo_sig){
	this->nodo_actual = nodo_sig;
}




//Buscar todos los casos similares al problema
char * Red::buscar(char problema[][LARGO_PROBLEMA], char tipo){

	char * resultado;
	Nodo * temp;
	Nodo ** nodo_encontrado;
	ListaAV * recorrido;
	ostringstream resultado_final;

	recorrido = new ListaAV();
	temp = raiz;
	double sim=0.0;
	//cout << "Buscando "<<endl;

	char ** problema_busqueda = new char*[this->cantidad_de_columnas];
	for(int i=0;i<cantidad_de_columnas;i++) problema_busqueda[i] = problema[i];

	switch(tipo){
		case 'H':
			buscarR(temp, problema_busqueda, &resultado_final, recorrido);
			break;
		case 'E':
			//cout << "Buscar exhaustivo"<<endl;
			nodo_encontrado = (Nodo **) malloc(sizeof(Nodo *));
			*nodo_encontrado = raiz;
			buscarEx(temp, problema_busqueda, nodo_encontrado);
			//cout << "fin buscar exhaustivo, nodo = "<<*nodo_encontrado<<endl;

			sim = similitud(*nodo_encontrado, problema_busqueda);
			resultado_final << "Encontrado caso "<<(*nodo_encontrado)->getParAV(0)[1]<<" con sim = "<<sim<<endl;
			resultado_final << "*** Caso = "<<(*nodo_encontrado)->verParesAV()<<" ***"<<endl;
			break;
		default:
			break;
	}

	int len = strlen(resultado_final.str().c_str());
	resultado = new char[len+1];
	strcpy(resultado, resultado_final.str().c_str());


	return resultado;
}

/*
 * Busqueda exhaustiva.  Recorre toda la red y busca el caso con mayor similitud.
 * */
void Red::buscarEx(Nodo * n, char ** problema, Nodo ** mejor_actual){
	int sucs = 0;
	double sim=0;
	double sim2=0;

	if(n->isa() != CASO){
		sucs = n->cantSucesores();
		//cout << "Nodo actual es un "<<n->isa_txt()<<" con "<<sucs<<" sucesores."<<endl;
		for(int i=0;i<sucs;i++){
			buscarEx(n->getSucesor(i), problema, mejor_actual);
		}
	}
	else{
		//cout << "Caso! Mejor caso actual = "<<mejor_actual<<endl;
		sim = this->similitud(n,problema);
		sim2 = similitud(*mejor_actual, problema);
		if(sim > sim2){
			*mejor_actual = n;
			//cout << "Nuevo mejor nodo = "<<(*mejor_actual)->getParAV(0)[1]<<" ("<<sim<<" > "<<sim2<<")"<<endl;
			//cout << "Addr "<<*mejor_actual<<endl;
		}
	}
}

/*
 * Busqueda Heuristica Recursiva --Profundidad Primero
 *
 * */
void Red::buscarR(Nodo * n, char ** problema, ostringstream * resultado, ListaAV * recorrido){
	int sucs = 0;
	double max_sim=0;
	double sim=0;
	int num_nodo=0;
	ListaAV * temp;

	if(n->isa() != CASO){
		sucs = n->cantSucesores();
		//cout << "Nodo actual es un "<<n->isa_txt()<<" con "<<sucs<<" sucesores."<<endl;
		for(int i=0;i<sucs;i++){

			//Probar rankeando TODOS los nodos
			//buscarR(n->getSucesor(i), problema, resultado);

			sim = this->similitud(n->getSucesor(i),problema);

			//cout << "Similitud para este caso es "<<sim<<endl;
			//cout << "["<< i << "] - ("<< sim<<") "<<n->getSucesor(i)->isa_txt() << n->getSucesor(i)->verParesAV() <<endl;

			if(sim > max_sim){
				max_sim = sim;
				num_nodo = i;
			}
		}
		//cout << "Elija un sucesor:";
		//cin >> num_nodo;
		//cout << endl;
		//cout << "Se selecciono el nodo tipo " << n->getSucesor(num_nodo)->isa_txt() <<endl;

		//Deberia haber una excepcion para las normas?
		if(n->isa() == INDICE)
			recorrido->agregar( ((Indice *) n)->getAtributo(), ((Indice *) n)->getValor(num_nodo), 0);
		else{
			//Si es norma, no necesariamente tiene ESTA cantidad de pares... entonces cual agrego?
			temp = ((Norma *) n)->getListaAV();
			for(int i=0;i<temp->longitud();i++){
				if(!recorrido->existeAtributoValor(temp->getParAV(i)))
					recorrido->agregar(temp->getParAV(i)[0],temp->getParAV(i)[1],0);
			}
			//recorrido->agregar( ((Norma *) n)->getParAV(num_nodo)[0], ((Norma *) n)->getParAV(num_nodo)[1], 0);
		}

		buscarR(n->getSucesor(num_nodo), problema, resultado, recorrido);
	}
	else{
		//cout << "CASO!"<<endl;
		sim = this->similitud(n,problema);
		num_nodo = atoi(n->getParAV(0)[1]);  //JourneyCode

		// Probar sin umbral

		//if(sim >= 0.6){//Definir umbral
			*resultado << "Encontrado caso "<<num_nodo<<" con sim = "<<sim<<endl;
			*resultado << "*** Caso = "<<n->verParesAV()<<" ***"<<endl;
			*resultado << "*** Recorrido: ";
			*resultado << recorrido->listarPares();
			*resultado << "***"<<endl;
		/*}
		else{
			*resultado << "Descartando caso " <<num_nodo<<" por baja similitud ("<<sim<<")"<<endl;
		}*/
	}
}

int Red::getCantidadCasos(){
	return this->cantidad_total_de_casos;
}

int Red::getCantidadDimensiones(){
	return this->cantidad_de_columnas;
}

char ** Red::getNombreDimensiones(){
	return this->nombre_columnas;
}

void Red::setDimensiones(char nombreDim[][LARGO_DIMENSION], int cantDim){
	this->nombre_columnas = new char*[cantDim];
	for(int i=0;i<cantDim;i++) this->nombre_columnas[i] = nombreDim[i];
	this->cantidad_de_columnas = cantDim;
}

Red * Red::dividir(){
	Nodo * temp_a1 = raiz;
	Nodo * temp_a2;
	Nodo * temp;
	int c = temp_a1->cantSucesores();
	Red * r2 = new Red(); //su raiz es una norma vacia
	temp_a2 = r2->getRaiz();
	int num_sucesor;
	int cantidad_de_veces;

	// Copiar todo hasta que c > 1
	// Hay que hacer una copia fisica, no solo el puntero pues quedaria compartido
	//cout << "Copiando nodos con 1 sucesor..."<<endl;
	while(c==1){
		//Copiar la lista de atributos del nodo en a1
		temp_a2->setListaAV(temp_a1->getListaAV());
		//Pasar el puntero al sucesor del nodo en a1
		temp_a1 = temp_a1->getSucesor(0);
		//Copiar el sucesor en la lista del nodo en a2
		switch(temp_a1->isa()){
			case INDICE:
				temp = new Indice(temp_a1->id_nodo());
				break;
			case NORMA:
				temp = new Norma(temp_a1->id_nodo());
				break;
			case CASO:
				temp = new Caso(temp_a1->id_nodo());
				break;
		}
		temp_a2->agregarSucesor(temp);
		//Pasar el puntero al sucesor del nodo en a2
		temp_a2 = temp_a2->getSucesor(0);
		//Obtener la cantidad de sucesores del nodo en a1
		c = temp_a1->cantSucesores();
	}

	//cout << "Fin nodos 1 sucesor.  Copiando >1 sucesores"<<endl;

	if(c > 1){
		//Copiar en r2 los sucesores de n/2 en adelante

		//temp_a1 es el ultimo nodo sin "hermanos", aca va el resto de la red

		//Si se trata de una norma, los pares a,v son compartidos y se copian todos
		if(temp_a1->isa() == NORMA){
			//cout << "Temp_a1 es una norma, voy a copiarle la listaAV completa..."<<endl;
			for(int i=0;i<temp_a1->getListaAV()->longitud();i++)
				temp_a2->agregarAV(temp_a1->getParAV(i)[0],temp_a1->getParAV(i)[1]);
		}

		//Copiar los sucesores en temp_a2 desde la mitad hasta el final, y eliminarlos de la lista de temp_a1
		num_sucesor = c/2;
		cantidad_de_veces = c - c/2;
		//cout << "Hay "<<c<<" sucesores en total, voy a copiar el sucesor "<<num_sucesor<<" "<<cantidad_de_veces<<" veces."<<endl;
		while(cantidad_de_veces > 0){
			temp_a2->agregarSucesor(temp_a1->getSucesor(num_sucesor));
			//cout << "Copiado sucesor "<<num_sucesor<<endl;
			if(temp_a1->isa() == INDICE){//Si se trata de un indice, copiar solo los que corresponden a los sucesores
				//cout << "Temp_a1 es un indice, voy a copiarle el parAV"<<endl;
				temp_a2->agregarAV(temp_a1->getParAV(num_sucesor)[0],temp_a1->getParAV(num_sucesor)[1]);
				//Lo siguiente elimina el sucesor Y el parAV
				//NOTA: cada vez que elimino un sucesor (o par a,v) se "corre a la izquierda" todo y la pos actual es en realidad el par-sucesor siguiente
				//cout << "Ahora voy a eliminar el sucesor"<<endl;
				temp_a1->eliminarSucesor(num_sucesor);
				//cout << "Sucesor y parAV eliminados."<<endl;
			}
			else{
				//Si es una norma, eliminar solo el sucesor y no los paresAV
				//cout << "Temp_a1 es una norma, solo eliminar sucesor."<<endl;
				temp_a1->eliminarSoloSucesor(num_sucesor);
				//cout << "Solo sucesor eliminado"<<endl;
			}

			cantidad_de_veces--;
			//cout << "Bien, quedan "<<cantidad_de_veces<<" sucesores por copiar"<<endl;
		}
		//Copiar los datos de las dimensiones
	//	r2->setDimensiones(this->getNombreDimensiones(), this->getCantidadDimensiones());
	}
	else{
		/* El arbol era una lista!
		 * En este caso c == 0 o c == 1.  Puede suceder:
		 * 1. No quedan mas sucesores, y se ha terminado de procesar
		 * 2. Sigue un nodo mas, hay que copiarlo
		 *
		 * Sin embargo, quedan 2 arboles iguales, y la division no tiene sentido.
		 */
		return 0;
	}

	return r2;
}

/*
 * Factor de ramificacion
 *
float Red::calcular_factorR(){
	Nodo * temp = raiz;
	int acc = 0;
	int nodos_visitados = 0;
	int sucs=0;

	calcular_factorR_rec(inicio, acc, nodos_visitados);

	Pila * p = new Pila();

	p->push(inicio,"raiz");

	while(!p->vacia()){
		nodos_visitados++;
		temp = ((Nodo *)p->topV());
		sucs = temp->cantSucesores();
		acc += sucs;

		for(int i=0;i<sucs; i++){

		}

	}

	cout << "Acc: " << *acc << ", Nodos: "<< nodos_visitados<<endl;

	return (float) *acc / (float) *nodos_visitados;
}

void Red::calcular_factorR_rec(Nodo * nodo, int * acc, int * nodos_visitados){
	int sucs = 0;
	if(nodo->isa() != CASO){
		sucs = nodo->cantSucesores();
		*acc += sucs;
		*nodos_visitados++;
		for(int i=0;i<sucs;i++)
			calcular_factorR_rec(nodo->getSucesor(i), acc, nodos_visitados);
	}
}*/
