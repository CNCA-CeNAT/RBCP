#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include "red.h"
#include "mpi.h"
#include "Grafo/parser.h"


//Hilos
#define MASTER 0
#define IMPRESOR 1
#define HILO_INICIAL 2

//Estructural
#define ARBOL 100
#define LONGITUD 110
#define RESULTADO 120
#define PROBLEMA 130
#define NUMDIM 140
#define NOMDIM 150

//Mensajes
#define MENSAJE 200
#define SOLUCION 210
#define SOLICITA_HILOS 220
#define LIBERAR_HILO 225
#define CANTIDAD_DE_HILOS 230
#define HILOS 240
#define PROCESAR 250
#define FINALIZAR 260
#define IMPRIMIR 270

//Estados
#define INACTIVO	0
#define ACTIVO	1

//Constantes
#define LARGO_BUFFER 4096
#define LARGO_DIMENSION 128

using namespace std;

int activos(int[], int);
int imprimir(char *);
int buscar_hilo_activo(int[], int);
int enviar_subarbol(char *, int);
int leerProblema(char *, char [][LARGO_DIMENSION], int);

int main(int argc, char ** argv){

  //Variables para el grafo
  Red * r;
  char * archivo;
  char * archivo_problema;
  Parser * p;

  //Variables para MPI
  MPI_Status Stat;
  int numtasks, rank, rc;

  char buffer[LARGO_BUFFER];
  char tipo_busqueda='H';

  /*
   * Iniciar MPI
   * */
  rc = MPI_Init(&argc,&argv);
  if (rc != MPI_SUCCESS) {
    cout << "Error al iniciar MPI." <<endl;
    MPI_Abort(MPI_COMM_WORLD, rc);
  }
  MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  if(argc >= 3){
	  archivo = argv[1];
	  archivo_problema = argv[2];
	  if(argc == 4)
		  if(argv[3][0] == 'H' || argv[3][0] == 'E')
			tipo_busqueda = argv[3][0];
		  else
			if(rank == MASTER)
				cout << "Tipo de busqueda incorrecto."<<endl;
  }
  else{
	  if(rank == MASTER){
		  cout << "Parametros insuficientes."<<endl;
		  cout << "Debe especificar un archivo con la coleccion de casos y uno con la descripcion del problema."<<endl;
		  cout << "Ademas puede especificar si la busqueda paralela es Exhaustiva o Heuristica."<<endl;
		  cout << "Sintaxis: "<< argv[0] <<" [archivo] [problema] [E|H]"<<endl;
		  for(int dest=1;dest<numtasks;dest++) rc = MPI_Send(buffer, LARGO_BUFFER, MPI_CHAR, dest, FINALIZAR, MPI_COMM_WORLD);
		  MPI_Finalize();
		  return 1;
	  }
  }

  //Inicializar variables del algoritmo
  int pila_hilos[numtasks];

  /*
   * Necesito obtener estos datos antes de declarar las matrices para dimensiones y el problema
   * */
  int numero_dimensiones;
  int cantidad_total_de_nodos;  //Permite implementar balanceo de cargas

  /**CODIGO PARALELO**/
  if(rank == MASTER){
    /** Procesar grafo, distribuirlo, etc... **/
    //Declarar e inicializar variables
    pila_hilos[MASTER] = ACTIVO;
    for(int i=1;i<numtasks;i++) pila_hilos[i] = INACTIVO;
    p = new Parser();

    /*
     * Por considerar: Desarrollar una forma de construir la red en forma distribuida, asignando a cada hilo una dimension-indice
     * diferente.
     * */
    double c1 = MPI_Wtime();
    imprimir("Construyendo red...");
    r = new Red();
    int exito = r->generar(archivo);
    double c2 = MPI_Wtime();
    if(exito == -1){
    	//sprintf(buffer,"Terminando.");
    	for(int dest=1;dest<numtasks;dest++) rc = MPI_Send(buffer, LARGO_BUFFER, MPI_CHAR, dest, FINALIZAR, MPI_COMM_WORLD);
    	MPI_Finalize();
    	return 1;
    }

    sprintf(buffer, "(%3.3f) Red construida - %i nodos.", c2-c1, r->contar_nodos());
    imprimir(buffer);

    sprintf(buffer, "Corriendo con %i procesos.", numtasks-HILO_INICIAL);
    imprimir(buffer);

    /*
     * Obtener el numero de dimensiones para declarar las matrices correspondientes del tamano adecuado en todos los procesos
     *
     * */
    numero_dimensiones = r->getCantidadDimensiones();
    cantidad_total_de_nodos = r->contar_nodos();
    	//imprimir(nombre_dimensiones[i]);
    }


	//*** Declaro matrices para problema y dimensiones con la cantidad de columnas leidas del archivo
	// ¿Es esta una solucion general? Quiza...
	MPI_Bcast(&cantidad_total_de_nodos, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
	MPI_Bcast(&numero_dimensiones, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
	char problema[numero_dimensiones][LARGO_DIMENSION];
	char nombre_dimensiones[numero_dimensiones][LARGO_DIMENSION];

    if(rank == MASTER){
    	char ** nombre_dimensiones_temp = r->getNombreDimensiones();
    	for(int i=0;i<numero_dimensiones;i++){
			strcpy(nombre_dimensiones[i],nombre_dimensiones_temp[i]);
    	}

    if(leerProblema(archivo_problema, problema, numero_dimensiones)){
    	imprimir("Problema cargado correctamente.");
    }
    else{
    	imprimir("Error al leer descripcion del problema");
    	sprintf(buffer,"Terminando.");
		for(int dest=1;dest<numtasks;dest++) rc = MPI_Send(buffer, LARGO_BUFFER, MPI_CHAR, dest, FINALIZAR, MPI_COMM_WORLD);
		MPI_Finalize();
		return 1;
    }


    imprimir("Parametros inicializados.");
  }

  MPI_Bcast(problema, LARGO_DIMENSION*10, MPI_CHAR, MASTER, MPI_COMM_WORLD);
  MPI_Bcast(nombre_dimensiones, LARGO_DIMENSION*10, MPI_CHAR, MASTER, MPI_COMM_WORLD);

  if(rank == MASTER){


	//¿Se puede borrar 'r' aca?
	//delete[] r;

	/*
	 * Se requiere una cantidad minima de hilos.
	 * El primero esta reservado para "Master"
	 * El segundo esta reservado para despliegue
	 * El tercero procesa la mitad izquierda
	 * El cuarto procesa la mitad derecha
	 * */
	if(numtasks < HILO_INICIAL+2){
	  //sprintf(buffer, "*** ERROR: Se necesitan al menos %i hilos para procesar este grafo.",HILO_INICIAL+2);


	  //Buscar con 1 hilo!!
	  double c1 = MPI_Wtime();
	  char * resultado = r->buscar(problema, tipo_busqueda);
	  double c2 = MPI_Wtime();

	  sprintf(buffer, "(%3.3f s.) Resultado:\n", c2-c1);
	  imprimir(buffer);
	  imprimir(resultado);

	  sprintf(buffer, "Finalizando.");
	  for(int dest=1;dest<numtasks;dest++) rc = MPI_Send(buffer, LARGO_BUFFER, MPI_CHAR, dest, FINALIZAR, MPI_COMM_WORLD);
	  MPI_Finalize();
	  return 1;
	}
	/******************************************************
	 *
	 * GENERAR SUBARBOLES Y HACER LA PRIMERA DISTRIBUCION
	 *
	 ******************************************************/
	//Para generar los subarboles
	//char * datos = r->generarLista();
	//char * subarbol;
	//Medir tiempos de ejecucion
	double c1, c2;

	char ** subarboles = new char*[2];//p->dividir(datos);
	int longitud;
	c1 = MPI_Wtime();  // /MPI_Wtick() ?
	Red * r2 = r->dividir();
	c2 = MPI_Wtime();
	sprintf(buffer,"(%3.3f s.) Subarbol dividido.",c2-c1);
	imprimir(buffer);

	c1 = MPI_Wtime();  // /MPI_Wtick() ?
	subarboles[0] = r->generarLista();
	subarboles[1] = r2->generarLista();
	c2 = MPI_Wtime();
	sprintf(buffer,"(%3.3f s.) Listas generadas.",c2-c1);
	imprimir(buffer);

	c1 = MPI_Wtime();  // /MPI_Wtick() ?
	longitud = strlen(subarboles[0]);
	//Activar el hilo
	rc = MPI_Send(buffer, LARGO_BUFFER, MPI_CHAR, HILO_INICIAL, PROCESAR, MPI_COMM_WORLD);
	//enviar longitud
	rc = MPI_Send(&longitud, 1, MPI_INT, HILO_INICIAL, LONGITUD, MPI_COMM_WORLD);
	//enviar datos
	rc = MPI_Send(subarboles[0], longitud, MPI_CHAR, HILO_INICIAL, ARBOL, MPI_COMM_WORLD);
	pila_hilos[HILO_INICIAL] = ACTIVO; //Marcar en uso

	longitud = strlen(subarboles[1]);
	//Activar el hilo
	rc = MPI_Send(buffer, LARGO_BUFFER, MPI_CHAR, HILO_INICIAL+1, PROCESAR, MPI_COMM_WORLD);
	//enviar longitud
	rc = MPI_Send(&longitud, 1, MPI_INT, HILO_INICIAL+1, LONGITUD, MPI_COMM_WORLD);
	//enviar datos
	rc = MPI_Send(subarboles[1], longitud, MPI_CHAR, HILO_INICIAL+1, ARBOL, MPI_COMM_WORLD);
	pila_hilos[HILO_INICIAL+1] = ACTIVO; //Marcar en uso

	c2 = MPI_Wtime();
	sprintf(buffer,"(%3.3f s.) Datos enviados.",c2-c1);
	imprimir(buffer);

	delete r;
	delete r2;

	imprimir("Buscando...");
	c1 = MPI_Wtime();
	//Escuchar solicitudes indefinidamente
    int tag = 0;
    int origen = 0;
    int long_result = 0;
    char * resultado_hilo;
    char * solucion = new char[256];
    int rango_hilo = 0;
    int cantidad_de_soluciones = 0;

    /*
     * Abrir archivo para generar estadisticas
     * */
    fstream salida;
    salida.open("estadisticas.dat", fstream::in | fstream::out | fstream::app);
    salida.setf(ios::fixed, ios::floatfield);
    salida.setf(ios::showpoint);
    //Desplegar cantidad de casos de la coleccion y cantidad de hilos de busqueda
    salida << r->getCantidadCasos() << "\t\t" << numtasks-HILO_INICIAL << "\t\t";

    while(activos(pila_hilos, numtasks)){
      //sprintf(buffer, "*** Quedan %i hilos activos.", activos(pila_hilos,numtasks));
      //imprimir(buffer);
      rc = MPI_Recv(buffer, LARGO_BUFFER, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &Stat);
      tag = Stat.MPI_TAG;
      origen = Stat.MPI_SOURCE;

      //Evaluar la solicitud
      switch(tag){
		case SOLUCION:
			//Recibir la solucion del hilo origen
			//sprintf(buffer,"Recibido resultado de %i",origen);
			//imprimir(buffer);
			rc = MPI_Recv(&long_result, 1, MPI_INT, origen, LONGITUD, MPI_COMM_WORLD, &Stat);
			//imprimir("Recibido largo");
			resultado_hilo = new char[long_result+1];
			rc = MPI_Recv(resultado_hilo, long_result, MPI_CHAR, origen, RESULTADO, MPI_COMM_WORLD, &Stat);
			//imprimir("Recibido dato");
			resultado_hilo[long_result]='\0';
			//imprimir("Fin recibido");
			c2 = MPI_Wtime();

			//Desplegar tiempo de primera solucion
			if(cantidad_de_soluciones == 0)
				salida << setprecision(3) << c2-c1 << "\t\t";

			sprintf(solucion, "(%3.3f s.) Resultado proceso %i:\n", c2-c1, origen);
			imprimir(solucion);
			if(long_result > 0){
			  //strcat(solucion, resultado_hilo);
				imprimir(resultado_hilo);
			  cantidad_de_soluciones++;
			}
			else
			  //strcat(solucion, "NO ENCONTRADO!");
				imprimir("NO ENCONTRADO");

			//Liberar el hilo en la pila
			pila_hilos[origen] = INACTIVO;
			break;
		case SOLICITA_HILOS:
			// El hilo origen necesita una cantidad de hilos, enviar los numeros de hilo asignados
			//y marcarlos como activos.
			rango_hilo = buscar_hilo_activo(pila_hilos, numtasks);
			if(!rango_hilo) imprimir("No hay procesos disponibles en este momento.");
			rc = MPI_Send(&rango_hilo, 1, MPI_INT, origen, HILOS, MPI_COMM_WORLD);
			pila_hilos[rango_hilo] = ACTIVO;
			break;
		case LIBERAR_HILO:
			pila_hilos[origen] = INACTIVO;
			break;
	  }
    }
    //MPI_Barrier(MPI_COMM_WORLD);
    c2 = MPI_Wtime();//clock() / (CLOCKS_PER_SEC / 1000);

    //Finalmente desplegar el tiempo total y la cantidad de soluciones
    salida << setprecision(3) << c2-c1 << "\t\t" << cantidad_de_soluciones<<endl;

    salida.close();

    //Broadcast no se puede por falta de TAG
    for(int dest=HILO_INICIAL;dest<numtasks;dest++) rc = MPI_Send(buffer, LARGO_BUFFER, MPI_CHAR, dest, FINALIZAR, MPI_COMM_WORLD);

    //Finalizar el hilo impresor e imprimir el mensaje final.
    sprintf(buffer, "Fin busqueda.\n%i procesos de busqueda, %i soluciones.\nTiempo total: %4.3f s.", numtasks-HILO_INICIAL, cantidad_de_soluciones, c2-c1);
    rc = MPI_Send(buffer, LARGO_BUFFER, MPI_CHAR, IMPRESOR, FINALIZAR, MPI_COMM_WORLD);

  }
  /*
   * CODIGO PARA EL HILO IMPRESOR
   *
   * */
  else
	  if(rank == IMPRESOR){
		  int tag = 1;
		  while(tag){
			  rc = MPI_Recv(buffer, LARGO_BUFFER, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &Stat);
			  switch(Stat.MPI_TAG){
				  case IMPRIMIR:
					  if(Stat.MPI_SOURCE == MASTER)
						  cout << "(MASTER): "<<buffer<<endl;
					  else
						  cout << Stat.MPI_SOURCE << ": "<<buffer<<endl;
					  break;
				  case FINALIZAR:
					  cout << "\n"<<buffer << "\n"<<endl;
					  tag = 0;
					  break;
			  }
		  }
	  }
  /*
   * CODIGO PARA TODOS LOS HILOS BUSCADORES (TODOS LOS DEMAS)
   *
   * */
  else{
    //recibir mensaje estandar de MASTER
    int final = 0;
    char * subarbol;
    char ** subarbol_dividido;
    int longitud = 0;
    char * resultado;
    Parser * p = new Parser();
    char * raiz;
    char ** sucesores;
    long cantSucesores;
    int long_resultado;
    int stat = -1;
    int origen = 0;
    int pila_nodos = 0;  //La cantidad de nodos que este hilo esta procesando
    int pila_ideal = 0;  //La cantidad de nodos que idealmente deberia tener
    int rango_hilo = 0;  //El numero del hilo asignado para dividir la carga
    int red_divisible = 1;

    Red * r2;

    while(!final){
      //cout << "Hilo " << rank << " esperando..."<<endl;
      rc = MPI_Recv(buffer, LARGO_BUFFER, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &Stat);
      //imprimir(itoa(Stat.MPI_TAG));
      stat = Stat.MPI_TAG;
      origen = Stat.MPI_SOURCE;

      switch(stat){
		case PROCESAR:
		//recibir longitud del subarbol
		//imprimir("Recibiendo arbol");
		  rc = MPI_Recv(&longitud, 1, MPI_INT, MPI_ANY_SOURCE, LONGITUD, MPI_COMM_WORLD, &Stat);
		  subarbol = new char[longitud];
		  //recibir subarbol
		  rc = MPI_Recv(subarbol, longitud, MPI_CHAR, MPI_ANY_SOURCE, ARBOL, MPI_COMM_WORLD, &Stat);

		  /*
		   * Determinar la cantidad de nodos en la pila de trabajo (subarbol actual) y calcular su tamano con respecto al arbol completo
		   *
		   * 1. Calcular la cantidad de nodos de la red actual
		   * 2. Determinar la relacion con respecto al tamano ideal
		   * 3. Si es mas grande, dividir en dos
		   * 4. Solicitar un hilo al MASTER y dividir la red en 2 subarboles
		   * 5. Enviar al nuevo proceso el segundo subarbol y que lo procese de la misma manera
		   *
		   * */
		  r = p->generarGrafo(subarbol);
		  pila_nodos = r->contar_nodos();//p->contarNodos(subarbol);
		  pila_ideal = cantidad_total_de_nodos / ((numtasks-HILO_INICIAL)*1.5);  //Como determinar el tamanno ideal?
		  red_divisible = 1;

		  /*!!!!
		   * Que pasa si quito del todo el criterio de "pila ideal" y el arbol se subdivide SIEMPRE que se pueda?
		   * */
		  while(pila_nodos > pila_ideal && red_divisible){ //(pila_nodos > pila_ideal) &&
		  /*
		   * Determinar si es apropiado dividir esta red en dos; puede estar muy mal balanceada
		   * */
		  //1. Solicita hilos al MASTER
			  rc = MPI_Send(buffer, LARGO_BUFFER, MPI_CHAR, MASTER, SOLICITA_HILOS, MPI_COMM_WORLD);
			  rc = MPI_Recv(&rango_hilo, 1, MPI_INT, MASTER, HILOS, MPI_COMM_WORLD, &Stat);

		  //2. Si hay algun proceso disponible, entonces enviarle su pila de nodos
			  if(rango_hilo){
				  r2 = r->dividir();
				  enviar_subarbol(r2->generarLista(), rango_hilo);
				  delete r2;
			  }
			  /* Que pasa si no hay hilos disponibles y se corre con el minimo... --no deberia esperar tanto */
			  else{
				  imprimir("Red no divisible. No hay procesos disponibles.");
				  red_divisible = 0;
			  }
			  pila_nodos = r->contar_nodos();
		  }

		  //Si no esta "vacia" --por ejemplo, algun error en las transformaciones
		  if(r->contar_nodos()){
			  //Procesar!
			  //sprintf(buffer, "Procesando red, %i < %i", pila_nodos, pila_ideal);
			  //imprimir(buffer);

			  //if(pila_nodos < (pila_ideal/10))
			 //   cout << "Subarbol proceso "<<rank<<" es muy corto ("<<pila_nodos<<" < "<<pila_ideal/10<<")"<<endl;

			  //imprimir("Generando grafo...");
			  //r = p->generarGrafo(subarbol);
			  //imprimir("Copiando dimensiones...");
			  r->setDimensiones(nombre_dimensiones, numero_dimensiones);

			  /*
			   * El overhead por comunicacion se puede justificar si la funcion de busqueda es suficientemente compleja
			   *
			   * Sino reducir el overhead --minimizar la cantidad de envios
			   *
			   * */
			  //imprimir("Iniciando busqueda...");
			  resultado = r->buscar(problema, tipo_busqueda);

			  //Notificar envio de solucion
			  rc = MPI_Send(buffer, LARGO_BUFFER, MPI_CHAR, MASTER, SOLUCION, MPI_COMM_WORLD);
			  //enviar longitud
			  long_resultado = strlen(resultado);
			  rc = MPI_Send(&long_resultado, 1, MPI_INT, MASTER, LONGITUD, MPI_COMM_WORLD);
			  //enviar resultado
			  rc = MPI_Send(resultado, long_resultado, MPI_CHAR, MASTER, RESULTADO, MPI_COMM_WORLD);

			  //sprintf(buffer, "Hilo %i finalizo busqueda", rank);
			  //imprimir(buffer);
		  }else{
			  //imprimir("Esta red no tiene nada!!");
			  rc = MPI_Send(buffer, LARGO_BUFFER, MPI_CHAR, MASTER, LIBERAR_HILO, MPI_COMM_WORLD);
		  }

		  break;
		case FINALIZAR:
			//cout << "Finalizando hilo "<< rank <<endl;
			//sprintf(buffer, "Hilo %i finalizando...", rank);
			//imprimir(buffer);
			final = 1;
			break;
      }
    }
    //sprintf(buffer, "Fin de hilo %i.",rank);
    //imprimir(buffer);
  }

  /**Fin CODIGO PARALELO**/

  MPI_Finalize();
  return 0;
}


/*
 * Envia texto al hilo desginado para impresion en pantalla
 * */
int imprimir(char * mensaje){
	MPI_Request send_req;
	return MPI_Isend(mensaje, LARGO_BUFFER, MPI_CHAR, IMPRESOR, IMPRIMIR, MPI_COMM_WORLD, &send_req);
}

/*
 * Recorre la pila de hilos y retorna la cantidad total de hilos marcados como ACTIVOS
 * */
int activos(int procesos[], int largo){
  int act = 0;

  for(int i=1; i<largo; i++)
    if(procesos[i])
      act++;

  //cout << "Hay "<<act<<" procesos activos."<<endl;

  return act;
}

/*
 * Ubica el numero del primer proceso inactivo
 *
 * Si retorna 0, no hay hilos disponibles
 *
 * */
int buscar_hilo_activo(int procesos[], int largo){
	int pos=0;

	for(int i=HILO_INICIAL;i<largo && !pos;i++)
		if(procesos[i] == INACTIVO)
			pos = i;

	return pos;
}

/*
 *
 * Enviar un arbol a un hilo y notificarle que lo procese
 *
 * */
int enviar_subarbol(char * subarbol, int dest){
	char buffer[LARGO_BUFFER];
	int longitud = strlen(subarbol);
	int rc;
	//Activar el hilo
	rc = MPI_Send(buffer, LARGO_BUFFER, MPI_CHAR, dest, PROCESAR, MPI_COMM_WORLD);
	//enviar longitud
	rc = MPI_Send(&longitud, 1, MPI_INT, dest, LONGITUD, MPI_COMM_WORLD);
	//enviar datos
	rc = MPI_Send(subarbol, longitud, MPI_CHAR, dest, ARBOL, MPI_COMM_WORLD);

	return rc;
}

int leerProblema(char * archivo, char problema [][LARGO_DIMENSION], int cols){
	ifstream arch;
	arch.open(archivo);

	if(!arch){
		cerr << "Error al abrir archivo \""<<archivo<<"\".\n"<<endl;
		return 0;
	}
	else{
		for(int i=0;i<cols;i++){
			arch.getline(problema[i],LARGO_DIMENSION);
		}
	}

	arch.close();

	char buffer[LARGO_BUFFER];
	sprintf(buffer,"Problema: (");
	for(int i=0;i<cols;i++)
		sprintf(buffer,"%s%s, ", buffer, problema[i]);
	sprintf(buffer,"%s)", buffer);
	imprimir(buffer);

	return 1;
}
