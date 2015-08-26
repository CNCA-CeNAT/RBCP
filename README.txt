*********************************************************
RBCp:  Razonamiento Basado en Casos en paralelo, para memoria distribuida.

	El objetivo de este programa es mostrar el algoritmo de recuperación de casos en paralelo (reconstrucción en RBC) sobre una memoria de casos distribuida, la cual utiliza una red de discriminación redundante balanceada en forma dinámica.  Publicación asociada:

J. C. Saborio-Morales and A. de la Ossa. Case-based reasoning in parallel environments. In F. Xhafa, L. Barolli, and K. F. Li, editors, 3PGCIC, pp. 327–332. IEEE, 2012.

*********************************************************
	Como prueba se utiliza la colección de casos del sistema CABATA, la cual contiene información sobre viajes.

	Para ejecutar el programa se requiere al menos 3 procesos y se recomienda utilizar MPICH/Hydra.
*********************************************************


1) Estructura:
	- Archivos .h y .cpp:	Fuente.
	- Archivos .csv:			Colecciones de casos.
	- Directorio Debug/:		Makefile, objetos y binario.

2) Compilación:

	Utilizar el 'makefile' incluido dentro del directorio 'Debug'.

2.1) Cambiar de directorio:
$cd Debug/

2.2) Compilar
$make

	La compilación utiliza el 'mpicxx' del sistema.  Se recomienda MPICH.

2.3) (Opcional) Copiar binario al directorio con los archivos de datos.
$cp RedDisc ..

3) Ejecución:

	Si se ejecuta el binario sin parámetros se obtiene el siguiente mensaje:
**
Parametros insuficientes.
Debe especificar un archivo con la coleccion de casos y uno con la descripcion del problema.
Ademas puede especificar si la busqueda paralela es Exhaustiva o Heuristica.
Sintaxis: ./RedDisc [archivo] [problema] [E|H]
**

3.1) El programa requiere al menos 3 procesos: uno admnistra recursos, otro despliega en pantalla, los demás para procesamiento.  Para ejecutar se recomienda utilizar Hydra.  Por ejemplo, para utilizar 4 procesos, los archivos 'casos.csv' y 'problema' con búsqueda Exhaustiva, se ejecuta:

$mpiexec.hydra -np 4 ./RedDisc casos.csv problema E

4) Modificación de valores de entrada:

4.1) Una colección de casos (como 'casos.csv') requiere el siguiente formato para ser leída correctamente:

--
[cantidad de casos]
[cantidad de dimensiones]
[lista de tipos de dimensiones]#  (valores = {STRING, INT, REAL})
[nombre de las dimensiones]#
casos#
--

Nota importante: excepto por las primeras dos líneas, todas deben finalizar con '#'.  Ejemplo:
--
1
10
INT;STRING;REAL;INT;STRING;STRING;INT;STRING;STRING;STRING#
JourneyCode;HolidayType;Price;NumberOfPersons;Region;Transportation;Duration;Season;Accommodation;Hotel#
1;Bathing;2498;2;Egypt;Plane;14;April;TwoStars;Hotel White House, Egypt#
--

4.2) Los problemas en RBC son esencialmente casos incompletos, cuya solución es el conjunto de casos con mayor similitud (más transformación).  Para especificar un problema simplemente se listan los valores deseados para cada dimensión posible en la colección, uno por línea, donde una línea vacía implica que no hay valor.  Ejemplo:

--

Bathing

2
Spain
Plane




--

Corresponde al problema: {HolidayType=Bathing, NumberOfPersons=2, Region=Spain, Transportation=Plane}.

5) Creditos y contacto:

Juan Carlos Saborio y Alvaro de la Ossa, 2012.
Email: jcsaborio@cenat.ac.cr, cnca@cenat.ac.cr
