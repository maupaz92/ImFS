/*
 * funciones_fuse.c
 *
 *  Created on: 30/11/2014
 *      Author: mauriciopaz
 */

#include "funciones_fuse.h"
#include "debug.h"
#include <errno.h>
#include <string.h>
#include <unistd.h>

/*
 * author maupaz92
 *
 * Función: las rutas en linux son dadas por = '/carpeta/carpeta/archivo.
 * 			La funcion se encarga  de contar cuantas apariciones del caracter
 * 			'/' existen en la ruta de entrada
 * ----------------------------------------------------------------------------------
 * Parámetros:
 * 			@param ruta
 * 				Ruta la cual se quiere saber la cantidad de niveles
 * 			@param
 *
 * 			@param
 *
 * 			@param
 *
 * ----------------------------------------------------------------------------------
 * Retorno en exito:
 * 		- cantidad de apariciones del caracter '/'
 * Retornos en error:
 * 		-
 * ----------------------------------------------------------------------------------
 */
int get_niveles_ruta(const char* ruta){
	int niveles;
	int conteo;
	for(conteo = 0, niveles = 0; ruta[conteo]; conteo++)
		niveles += (ruta[conteo] == '/');
	return niveles;
}



/*
 * author maupaz92
 *
 * Función: construye un arreglo de nombres segun la ruta que ingrese como parametro.
 * 			Para una ruta /carp1/carp2/archivo, se retornaria un arreglo de para cada
 * 			uno de los nombres: carp1, carp2, archivo. DEBE DE LIBERARSE LA MEMORIA
 * 			correspondiente para el arreglo de arreglos, es responsabilidad del que
 * 			llama la funcion.
 * ----------------------------------------------------------------------------------
 * Parámetros:
 * 			@param niveles
 * 				cantidad de apariciones del caracter "/"
 * 			@param ruta
 * 				ruta absoluta que de la cual se quiere saber los nombres que la contienen
 * 			@param
 *
 * 			@param
 *
 * ----------------------------------------------------------------------------------
 * Retornos:
 * 		- arreglo doble de punteros 'char'. El primer arreglo apunta hacia cada arreglo
 * 		de nombres.
 * 		- NULL en caso de que la ruta sea root '/'
 *
 * ----------------------------------------------------------------------------------
 */
char** get_nombres_ruta(int niveles, const char* ruta){
	size_t longitud_ruta = strlen(ruta);
	char copia_ruta[longitud_ruta];
	strcpy(copia_ruta, ruta);
	char* tokens = strtok(copia_ruta, "/");
	if(tokens == NULL)
		return NULL;
	char** nombres = (char**) malloc(niveles * sizeof(char*));
	int conteo;
	for(conteo = 0; conteo < niveles; conteo++){
		nombres[conteo] = (char*) malloc(LONGITUD_MAXIMA_NOMBRE_ARCHIVO * sizeof(char));
	}
	conteo = 0;
	while(tokens != NULL){
		strcpy(nombres[conteo], tokens);
		tokens = strtok(NULL, "/");
		conteo++;
	}
	return nombres;
}



/*
 * author maupaz92
 *
 * Función: Obtiene el nombre de un archivo/directorio que se este trabajando
 * 			a partir del arreglo de nombres que ingrese como parametro.
 * 			el nombre siempre corresponde al ultimo elemento de la ruta,
 * 			/usr/local/a.o = a.o es el nombre. Se debe desalojar la memoria
 * 			obtenida para el arreglo de caracteres luego de su uso-
 * ----------------------------------------------------------------------------------
 * Parámetros:
 * 			@param niveles_ruta
 * 				niveles de la ruta
 * 			@param nombres_niveles
 * 				arreglo de todos los nombres de la ruta
 * 			@param
 *
 * 			@param
 *
 * ----------------------------------------------------------------------------------
 * Retornos:
 * 		- arreglo de caracteres con el nombre del archivo.
 * 		-
 * 		-
 * ----------------------------------------------------------------------------------
 */
char* get_nombre_archivo(int niveles_ruta, char** nombres_niveles){
	char* nombre_archivo;
	size_t largo_nombre = strlen(nombres_niveles[niveles_ruta - 1]);
	nombre_archivo = (char*) malloc(largo_nombre * sizeof(char));
	strcpy(nombre_archivo, nombres_niveles[niveles_ruta - 1]);
	return nombre_archivo;
}



/*
 * author maupaz92
 *
 * Función: Des-asigna memoria del arreglo de caracteres que se utiliza
 * 			para descomponer la ruta con la que se trabaja. Como se debe
 * 			de implementar en casi todas las funciones, se separa.
 * ----------------------------------------------------------------------------------
 * Parámetros:
 * 			@param niveles
 * 				niveles de la ruta
 * 			@param nombres
 * 				arreglo de nombres separados por strtok
 * 			@param
 *
 * 			@param
 *
 * ----------------------------------------------------------------------------------
 * Retornos:
 * 		-
 * 		-
 * 		-
 * ----------------------------------------------------------------------------------
 */
void desalojar_memoria_nombres_ruta(int niveles, char** nombres){
	//se desaloja la memoria solicitada
	int conteo;
	for(conteo = 0; conteo < niveles; conteo++)
		free(nombres[conteo]);
	free(nombres);
}



int get_cantidad_maxima_archivos_en_directorio(){
	return (TAMANIO_BLOQUE_BYTES - sizeof(encabezado_directorio)) /sizeof(mapeo_archivo);
}



posicion_memoria get_posicion_relativa_en_unidad(byte* puntero){
	posicion_memoria resultado = ((posicion_memoria) puntero) - referencia_inicial_memoria;
	return resultado;
}



byte* get_dato_en_posicion_relativa(posicion_memoria posicion_relativa){
	if(posicion_relativa == 0)
		return NULL;
	byte* resultado = unidad_almacenamiento_fs + posicion_relativa;
	return resultado;
}



uint get_posicion_bloque_segun_numero(uint numero_bloque){
	return TAMANIO_BLOQUE_BYTES*(numero_bloque - 1);
}



referencia_archivo* get_referencia_root(){

	encabezado_referencias* encabezado_ref_inicial =
			(encabezado_referencias*) get_dato_en_posicion_relativa(super_block->p_inicio_referencias_archivos);
	referencia_archivo* ref_root =
			(referencia_archivo*) get_dato_en_posicion_relativa(encabezado_ref_inicial->p_referencia_inicial);
	return ref_root;
}



encabezado_directorio* get_directorio_root(){
	referencia_archivo* root_ref = get_referencia_root();
	int posicion_directorio = get_posicion_bloque_segun_numero(root_ref->bloques_de_datos[0]);
	return ((encabezado_directorio*)(unidad_almacenamiento_fs + posicion_directorio));
}



encabezado_directorio* get_directorio_para_archivo(int niveles_ruta, char** nombres_niveles){
	int copia_niveles = niveles_ruta;
	// se inicia en el root
	encabezado_directorio* direct = get_directorio_root();
	int nivel = 0;
	int limite = get_cantidad_maxima_archivos_en_directorio();

	while(copia_niveles != 1){
		//se itera sobre tod_o el directorio
		int conteo;
		int bandera_abortar = 0;
		referencia_archivo* ref;
		//se obtiene el mapeo inicial del directorio actual
		mapeo_archivo* inicial = (mapeo_archivo*) get_dato_en_posicion_relativa(direct->p_inicio_mapeos) ;
		//se cuenta primero sobre los registro del struct
		for(conteo = 0; (conteo < limite) && (bandera_abortar == 0); conteo++){
			mapeo_archivo* mapeo = inicial + conteo;
			int comparacion = strcmp(nombres_niveles[nivel], mapeo->nombre);
			//si los nombres son iguales, se encontro el mapeo
			if(comparacion == 0){
				bandera_abortar = 1;
				//se actualiza la referencia por la del mapeo encontrado
				ref = (referencia_archivo*) get_dato_en_posicion_relativa(mapeo->p_referencia);
				//se actualiza el directorio a partir del mapeo
				direct = (encabezado_directorio*)(unidad_almacenamiento_fs + get_posicion_bloque_segun_numero(ref->bloques_de_datos[0]));
			}
		}
		/**---- TODO: verificar que la referencia que se obtenga sea un directorio  ---**/
		copia_niveles--;
		nivel++;
	}
	DEBUG_MENSAJE_UN_ARGUMENTO("Se obtuvo directorio: [%s]", direct->nombre_directorio);
	return direct;
}



/*
 * author maupaz92
 *
 * Función: Busca la referencia del archivo/directorio especificada por 'nombres_ruta'.
 * 			Se busca sobre el directorio de la ruta todos las estructuras 'mapeo' el
 * 			nombre que corresponde al archivo/directorio que se busca. Si hay exito, se
 * 			retorna el puntero a la referencia del archivo/directorio, de lo contrario
 * 			se retorna NULL.
 * ----------------------------------------------------------------------------------
 * Parámetros:
 * 			@param
 *
 * 			@param
 *
 * 			@param
 *
 * 			@param
 *
 * ----------------------------------------------------------------------------------
 * Retornos:
 * 		-
 * 		-
 * 		-
 * ----------------------------------------------------------------------------------
 */
referencia_archivo* get_referencia_archivo(){
	referencia_archivo* ref = NULL;
	//se obtiene el archivos a partir de la ruta
	encabezado_directorio* direct_actual = get_directorio_para_archivo(cantidad_niveles, nombres_niveles);
	//se obtiene el nombre segun la ruta
	char* nombre_registro = get_nombre_archivo(cantidad_niveles, nombres_niveles);
	int cantidad_registro = get_cantidad_maxima_archivos_en_directorio();
	int conteo;
	int bandera_continuar = 1;
	//se itera sobre la cantidad de archivos que tenga el directorio
	for(conteo = 0; conteo < cantidad_registro && bandera_continuar == 1; conteo++){
		mapeo_archivo* map = ((mapeo_archivo*) get_dato_en_posicion_relativa(direct_actual->p_inicio_mapeos)) + conteo;
		//si uno de los archivos coincide en nombre
		if(map->estado == 1 && strcmp(nombre_registro, map->nombre) == 0){
			DEBUG_MENSAJE_UN_ARGUMENTO("Obtenido un archivo/directorio: [%s]", map->nombre);
			bandera_continuar = 0;
			ref = (referencia_archivo*) get_dato_en_posicion_relativa(map->p_referencia);
		}
	}
	free(nombre_registro);
	return ref;
}



encabezado_referencias* get_encabezado_con_espacio(int* posicion_nueva_ref){

	int posicion_referencia = -1;
	posicion_nueva_ref = &posicion_referencia;
	//se obtiene el primer encabezado de referencias
	encabezado_referencias* encabezado_refs = (encabezado_referencias*) get_dato_en_posicion_relativa(super_block->p_inicio_referencias_archivos);
	int bandera_cupo_lleno = 0;
	int bandera_buscar_siguiente = 0;
	int bandera_sin_espacio = 0;
	int contador = 1;
	do{
		/**
		 * se itera sobre los encabezados de referencias para encontrar el siguiente
		 * que tenga espacio
		 */
		DEBUG_MENSAJE_UN_ARGUMENTO("Buscando sobre encabezado [%d], un espacio de referencia disponible", contador);
		bandera_cupo_lleno = 0;
		bandera_buscar_siguiente = 0;
		int actuales = encabezado_refs->cantidad_actual_referencias;
		int maximo = get_cantidad_maxima_archivos_en_directorio();
		int referencias_fantasma = encabezado_refs->cantidad_referencias_fantasma;
		DEBUG_MENSAJE_TRES_ARGUMENTOS("Tiene [%d] referencias ocupadas de [%d], y [%d] fantasmas", actuales, maximo, referencias_fantasma);
		encabezado_referencias* siguiente = (encabezado_referencias*) get_dato_en_posicion_relativa(encabezado_refs->p_siguiente);

		if(referencias_fantasma == 0){
			DEBUG_MENSAJE("Encabezado sin referencias fantasmas");
			//si ya no puede manejar mas referencias, tiene tod_o su espacio ocupado
			if(actuales == maximo){
				DEBUG_MENSAJE("Encabezado lleno, sin mas espacio para referencias");
				bandera_cupo_lleno = 1;
			}

			if(bandera_cupo_lleno == 1){
				//si tiene un siguiente encabezado de referencias
				if(siguiente != NULL){
					DEBUG_MENSAJE("Se buscara sobre un siguiente header de referencias");
					bandera_buscar_siguiente = 1;
					encabezado_refs = siguiente;
				}else{
					//si ya no hay mas encabezados disponibles para iterar
					bandera_sin_espacio = 1;
					DEBUG_MENSAJE("No hay mas espacio en los headers actuales");
				}
			}
			contador++;
		}else{

//			DEBUG_MENSAJE_UN_ARGUMENTO("Se utilizara una referencia fantasma, tiene: [%d]", referencias_fantasma);
//			int conteo;
//			int continuar_busqueda = 1;
//			for(conteo = 0; (conteo < actuales) && (continuar_busqueda == 1); conteo++){
//				referencia_archivo* ref_siguiente = encabezado_refs->referencia_inicial + conteo;
//				//si la ref actual es fantasma
//				if(ref_siguiente->tipo_archivo == FANTASMA){
//					//se define su posicion a partir de la referencia inicial del encabezado
//					posicion_referencia = conteo;
//					continuar_busqueda = 0;
//				}
//			}
		}

	}while(bandera_buscar_siguiente);

	if(bandera_sin_espacio == 1){ //cuando no hay espacio, se debe de crear uno nuevo
		DEBUG_MENSAJE_UN_ARGUMENTO("No hay espacio disponible en el header ref actual, se asigna un nuevo encabezado en el bloque [%d]",
				super_block->siguiente_bloque_referencias);
		encabezado_referencias* nuevo_header = (encabezado_referencias*)
				(unidad_almacenamiento_fs + get_posicion_bloque_segun_numero(super_block->siguiente_bloque_referencias));
		//se actualiza el valor para la posicion del siguiente encabezado
		super_block->siguiente_bloque_referencias++;
		nuevo_header->cantidad_actual_referencias = 0;
		nuevo_header->cantidad_bloques_fantasma = 0;
		nuevo_header->cantidad_referencias_fantasma = 0;
		nuevo_header->p_referencia_inicial = get_posicion_relativa_en_unidad(((byte*) nuevo_header) + sizeof(encabezado_referencias));
		nuevo_header->p_siguiente = 0;
		//se asigna como siguiente al nuevo encabezado
		encabezado_refs->p_siguiente = get_posicion_relativa_en_unidad((byte*) nuevo_header);
		encabezado_refs = nuevo_header;
	}
	*posicion_nueva_ref = posicion_referencia;
	return encabezado_refs;
}


/*
 * author maupaz92
 *
 * Función: Busca sobre todos los encabezados y sus referencias algun bloque
 * 			que no este en uso: "fantasma". La busqueda inicia primero con la
 * 			referencia que ingresa como parametro, esto para agilizar la busqueda
 * 			si tuviese uno libre. Si la referencia no tiene ningun bloque libre,
 * 			se itera sobre los encabezados, si tiene nocion de que hay bloques
 * 			fantasmas, se itera sobre las referencias hasta encontrar el bloque.
 * ----------------------------------------------------------------------------------
 * Parámetros:
 * 			@param referencia
 * 				referencia que esta solicitando un nuevo bloque
 * 			@param
 *
 * 			@param
 *
 * 			@param
 *
 * ----------------------------------------------------------------------------------
 * Retornos:
 * 		- Si se encuentra un bloque fantasma en el sistema de archivos, se retorna
 * 		el numero del mismo.
 * 		- si no se encuentran bloques libres, se retorna 0
 * 		-
 * ----------------------------------------------------------------------------------
 */
uint get_bloque_fantasma(referencia_archivo* referencia){

	uint bloque = 0;
	//primero revisa que la refencia que lo solicita no tenga uno
	if(referencia->bloques_fantasma > 0){
		DEBUG_MENSAJE("La referencia tiene un bloque fantasma disponible");
//		bloque = referencia->bloques_de_datos[referencia->bloques_asignados];
//		referencia->bloques_asignados++;
//		referencia->bloques_fantasma--;
//		encabezado_referencias* contenedor = ((encabezado_referencias*) get_dato_en_posicion_relativa(referencia->p_encabezado_contenedor));
//		contenedor->cantidad_bloques_fantasma--;
	}else{
//		//se obtiene la cantidad de encabezados en disco, no se incluye el superbloque, ni el inicial
//		uint cantidad_encabezados = super_block->siguiente_bloque_referencias - 2;
//		uint conteo;
//		//se inicia a partir del encabezado primero
//		encabezado_referencias* encabezado = ((encabezado_referencias*) get_dato_en_posicion_relativa(super_block->p_inicio_referencias_archivos));
//				//super_block->inicio_referencias_archivos;
//		byte max_referencias = encabezado->cantidad_maxima_referencias;
//		int bandera_continuar = 1;
//		for(conteo = 0; conteo < cantidad_encabezados && bandera_continuar == 1; conteo++){
//			if(encabezado->cantidad_bloques_fantasma > 0){
//				//se obtiene la primera referencia del encabezado
//				referencia_archivo* ref = encabezado->referencia_inicial;
//				int contador;
//				//se itera sobre todas las referencias del mismo
//				for(contador = 0; contador < max_referencias; contador++){
//					//si la actual referencia tiene bloques fantasma
//					if(ref->bloques_fantasma > 0){
//						bloque = ref->bloques_de_datos[ref->bloques_totales - 1];
//						ref->bloques_totales--;
//						ref->bloques_fantasma--;
//						encabezado->cantidad_bloques_fantasma--;
//						bandera_continuar = 0;
//					}
//					ref = ref + contador;
//				}
//			}else{
//				//se pasa al siguiente encaezado
//				encabezado = encabezado + 1;
//			}
//		}
	}
	DEBUG_MENSAJE_UN_ARGUMENTO("Obtenido el bloque fantasma: [%d]", bloque);
	return bloque;
}



void agregar_referencia_a_directorio(referencia_archivo* referencia, char* nombre, encabezado_directorio* direct){
	//se supone que el directorio tiene espacio, actual o extra
	int bandera_continuar = 0;
	encabezado_directorio* d_comodin = direct;
	mapeo_archivo* mapeo;
	int conteo;
	int limite = get_cantidad_maxima_archivos_en_directorio();
	int continuar = 1;
	DEBUG_MENSAJE_DOS_ARGUMENTOS("Agregando el archivo [%s] en directorio [%s]", nombre, direct->nombre_directorio);
	do{
		//si hay espacio en el directorio
		if(d_comodin->cantidad_actual_archivos < limite){
			//se itera sobre el mismo
			for(conteo = 0; conteo < limite && continuar == 1  ; conteo++){
				mapeo = ((mapeo_archivo*) get_dato_en_posicion_relativa(d_comodin->p_inicio_mapeos)) + conteo;

				if(mapeo->estado == 0){
					DEBUG_MENSAJE_UN_ARGUMENTO("agregado el archivo al directorio en la posicion [%d]", conteo);
					continuar = 0;
					strcpy(mapeo->nombre, nombre);
					mapeo->estado = 1;
					mapeo->p_referencia = get_posicion_relativa_en_unidad((byte*) referencia);
					d_comodin->cantidad_actual_archivos++;
				}
			}
		}
		else if(d_comodin->p_directorio_extra != 0){
			DEBUG_MENSAJE("El directorio tiene una lista extra, se buscara sobre la misma");
//			d_comodin = d_comodin->directorio_extra;
		}else{
			DEBUG_ERORR("El directorio no tiene mas campo, y no se ha agregado mas espacio para el mismo");
		}
	}while(bandera_continuar == 1);
}



referencia_archivo* agregar_nuevo_archivo(const char* ruta, tipo_archivo tipo){

	uint bloques_restantes = super_block->cantidad_bloques - super_block->cantidad_bloques_ocupados;
	DEBUG_MENSAJE_UN_ARGUMENTO("Cantidad de bloques restantes: [%d]", bloques_restantes);
	if(bloques_restantes <= 2){
		DEBUG_ERORR("Sistema de archivos lleno, sin espacio para mas datos");
		/**---- TODO:revisar que pasa cuando no hay espacio  ---**/
		return NULL;
	}
	cantidad_niveles = get_niveles_ruta(ruta);
	nombres_niveles = get_nombres_ruta(cantidad_niveles, ruta);
	nombre_archivo = get_nombre_archivo(cantidad_niveles, nombres_niveles);

	/**---- TODO:verificar que el nombre no tenga mas del tamño permitido  ---**/

	encabezado_directorio* dir_owner;
	//se busca el directorio segun la ruta, para guardar el archivo
	dir_owner = get_directorio_para_archivo(cantidad_niveles, nombres_niveles);
	desalojar_memoria_nombres_ruta(cantidad_niveles, nombres_niveles);
	int valor = -1;
	int* posicion_siguiente_referencia = &valor;
	encabezado_referencias* encabezado_refs = get_encabezado_con_espacio(posicion_siguiente_referencia);
	referencia_archivo* nueva_ref;

	if(*posicion_siguiente_referencia == -1){ //si hay que crear una nueva referencia

		DEBUG_MENSAJE("Creando una nueva referencia, para el nuevo archivo");
		//se obtiene la posicion de la nueva referencia
		byte posicion_nueva_ref = encabezado_refs->cantidad_actual_referencias;
		nueva_ref = ((referencia_archivo*) get_dato_en_posicion_relativa(encabezado_refs->p_referencia_inicial)) + posicion_nueva_ref;
		nueva_ref->bloques_fantasma = 0;
		nueva_ref->bloques_totales = 0;
		encabezado_refs->cantidad_actual_referencias++;
		nueva_ref->bloques_asignados = 0;

	}else{ // si hay que utilizar una referencia del mismo encabezado, una fantasma
		DEBUG_MENSAJE("Se va a utilizar una referencia ya existente, fantasma");
//		nueva_ref = encabezado_refs->referencia_inicial + *posicion_siguiente_referencia;
		//nueva_ref->bloques_fantasma += nueva_ref->bloques_asignados;
	}

	nueva_ref->p_encabezado_contenedor = get_posicion_relativa_en_unidad((byte*) encabezado_refs);
	nueva_ref->tipo_archivo = tipo;
	nueva_ref->tamanio = 0;
	nueva_ref->p_referencia_extra = 0;

	/**---- TODO: verificar que el directorio tenga campo, sino crear un campo extra  ---**/
	agregar_referencia_a_directorio(nueva_ref, nombre_archivo, dir_owner);
	DEBUG_MENSAJE_DOS_ARGUMENTOS("Cantidad de elementos: [%d] en directorio [%s]", dir_owner->cantidad_actual_archivos, dir_owner->nombre_directorio);
	return nueva_ref;
}



void crear_sistema_archivos(int tamanio_unidad){

	referencia_inicial_memoria = (posicion_memoria) unidad_almacenamiento_fs;
	unsigned long bloques = tamanio_unidad / TAMANIO_BLOQUE_BYTES;
	//se calcula el tamaño del struct que contiene la info del FS gral
	unsigned long tamanio_super_block = sizeof(super_bloque);
	//se calcula el tamaño del struct q mantiene la info de un conjunto de referencias
	unsigned long tamanio_encabezado_refs = sizeof(encabezado_referencias);
	//Se calcula el tamaño de una referencia a un archivo/directorio
	unsigned long tamanio_referencia = sizeof(referencia_archivo);
	//se calcula el tamaño que resta del bloque las referencias
	unsigned long campo_restante = TAMANIO_BLOQUE_BYTES - tamanio_encabezado_refs;
	unsigned long tamanio_directorio = sizeof(encabezado_directorio);
	unsigned long tamanio_mapeo = sizeof(mapeo_archivo);

	DEBUG_MENSAJE_UN_ARGUMENTO("tamaño de la unidad de memoria: [%d] bytes", tamanio_unidad);
	DEBUG_MENSAJE_UN_ARGUMENTO("tamaño de los bloques: [%d] bytes", TAMANIO_BLOQUE_BYTES);
	DEBUG_MENSAJE_UN_ARGUMENTO("cantidad de bloques [%lu], tamaño total/tamaño bloque", bloques);
	DEBUG_MENSAJE_UN_ARGUMENTO("tamaño del struct super block [%lu] bytes", tamanio_super_block);
	DEBUG_MENSAJE_UN_ARGUMENTO("tamaño del struct encabezado de referencias [%lu] bytes", tamanio_encabezado_refs);
	DEBUG_MENSAJE_UN_ARGUMENTO("tamaño del struct referencia archivo/directorio [%lu] bytes", tamanio_referencia);
	DEBUG_MENSAJE_UN_ARGUMENTO("tamaño del struct directorio [%lu] bytes", tamanio_directorio);
	DEBUG_MENSAJE_UN_ARGUMENTO("tamaño del struct mapeo [%lu] bytes", tamanio_mapeo);
	DEBUG_MENSAJE_UN_ARGUMENTO("cantidad de referencias disponibles en primer encabezado [%lu]", campo_restante/tamanio_referencia);

	//se define el espacio para el encabezado del FS.
	super_block = (super_bloque*) unidad_almacenamiento_fs;
	super_block->largo_total_unidad = TAMANIO_BYTES_SISTEMA_ARCHIVOS;
	super_block->cantidad_bloques = bloques;
	//al inicio el de referencias y el del directorio root, superblock
	super_block->cantidad_bloques_ocupados = 3;
	//numero de bloque siguiente para referencias
	super_block->siguiente_bloque_referencias = 3;
	super_block->siguiente_bloque_datos = bloques;
	super_block->espacio_disponible = (bloques - 3) * TAMANIO_BYTES_SISTEMA_ARCHIVOS;

	//se define la posicion del inicio de los encabezados
	super_block->p_inicio_referencias_archivos = get_posicion_relativa_en_unidad(unidad_almacenamiento_fs + TAMANIO_BLOQUE_BYTES);
	byte* encabezado_inicial = get_dato_en_posicion_relativa(super_block->p_inicio_referencias_archivos);

	// SE CREA LA REFERENCIA PARA EL DIRECTORIO ROOT
	//se obtiene el puntero de la primer referencia
	referencia_archivo* ref_root = (referencia_archivo*) (encabezado_inicial + tamanio_encabezado_refs);
	ref_root->tipo_archivo = DIRECTORIO;
	ref_root->bloques_totales = 1;
	ref_root->bloques_fantasma = 0;
	ref_root->bloques_asignados = 1;
	ref_root->p_encabezado_contenedor = get_posicion_relativa_en_unidad(encabezado_inicial);
	ref_root->tamanio = 0;
	//se define el primer bloque de la referencia
	ref_root->bloques_de_datos[0] = super_block->siguiente_bloque_datos;

	ref_root->p_referencia_extra = 0;

	//SE CREA EL DIRECTORIO, Y SE ASIGNA A UNA POSICION AL FINAL DE LOS BLOQUES

	//se calcula la posicion del siguiente bloque de datos
	size_t offset = (super_block->siguiente_bloque_datos - 1) * TAMANIO_BLOQUE_BYTES;
	// se actualiza el siguiente numero de bloque de datos
	super_block->siguiente_bloque_datos--;

	//se obtiene el puntero a la posicion segun el calculo anterior
	encabezado_directorio* root = (encabezado_directorio*) (unidad_almacenamiento_fs + offset);
	char nombre_root[] = "/";
	strcpy(root->nombre_directorio, nombre_root);
	root->cantidad_actual_archivos = 0;
	root->p_referencia = get_posicion_relativa_en_unidad((byte*)ref_root);
	root->p_inicio_mapeos = get_posicion_relativa_en_unidad(((byte*) root) + sizeof(encabezado_directorio));
	root->p_directorio_extra = 0;

	DEBUG_MENSAJE_UN_ARGUMENTO("Cantidad maxima de archivos soportados por un struct: [%d]",
	get_cantidad_maxima_archivos_en_directorio());

	encabezado_referencias* inicio_refs = (encabezado_referencias*) encabezado_inicial;
	inicio_refs->cantidad_actual_referencias = 1;
	inicio_refs->cantidad_referencias_fantasma = 0;
	inicio_refs->cantidad_bloques_fantasma = 0;
	inicio_refs->p_referencia_inicial = get_posicion_relativa_en_unidad((byte*)ref_root);
	inicio_refs->siguiente = NULL;
}



int imgfs_getattr(const char* ruta, struct stat* stbuf){

	int resultado = 0;
	int niveles_ruta = get_niveles_ruta(ruta);
	char** nombres_niveles = get_nombres_ruta(niveles_ruta, ruta);
	//se limpia la estructura 'stbuf', se pone '0' en todos sus datos
	memset(stbuf, 0, sizeof(struct stat));

	/**
	 *  TODO: verificar que esto funciones cuando las rutas son mas grandes /al/al/al x ej
	 */

	if(nombres_niveles != NULL){
		DEBUG_MENSAJE_UN_ARGUMENTO("Buscando atributos de registro, ruta [%s]", ruta);
		//se obtiene el directorio a partir de la ruta
		encabezado_directorio* direct_actual = get_directorio_para_archivo(niveles_ruta, nombres_niveles);
		//se obtiene el nombre segun la ruta
		char* nombre_registro = get_nombre_archivo(niveles_ruta, nombres_niveles);
		int cantidad_registro = get_cantidad_maxima_archivos_en_directorio();
		int conteo;
		int bandera_archivo_no_existe = 1;

		//se itera sobre la cantidad de archivos que tenga el directorio
		for(conteo = 0; conteo < cantidad_registro; conteo++){
			mapeo_archivo* map = ((mapeo_archivo*) get_dato_en_posicion_relativa(direct_actual->p_inicio_mapeos)) + conteo;
			//si uno de los archivos coincide en nombre
			if(map->estado == 1 && strcmp(nombre_registro, map->nombre) == 0){
				bandera_archivo_no_existe = 0;
				DEBUG_MENSAJE_UN_ARGUMENTO("Encontrado registro: [%s]", nombre_registro);
				//se obtiene su referencia
				referencia_archivo* ref = ((referencia_archivo*) get_dato_en_posicion_relativa(map->p_referencia));
				stbuf->st_uid = getuid();
				stbuf->st_gid = getgid();
				if(ref->tipo_archivo == ARCHIVO){
					stbuf->st_mode = S_IFREG | 0644;
					stbuf->st_nlink = 1;
					stbuf->st_size = ref->tamanio;
				}else if (ref->tipo_archivo == DIRECTORIO){
					stbuf->st_mode = S_IFDIR | 0755;
					stbuf->st_nlink = 2;
				}else{
					DEBUG_ERORR("El archivo es de tipo indefinido");
				}
			}
		}
		if(bandera_archivo_no_existe){
			DEBUG_MENSAJE("El archivo no esta registrado en el sistema de archivos");
			resultado = -ENOENT;
		}
		free(nombre_registro);
		desalojar_memoria_nombres_ruta(niveles_ruta, nombres_niveles);
	}else{
		/**---- TODO: agregar mas informacion  ---**/
		DEBUG_MENSAJE("Definiendo los atributos de la carpeta root");
		//modo del directorio,
        stbuf->st_mode = S_IFDIR | 0755;
        //cantidad de hard links
        stbuf->st_nlink = 2;
	}
	return resultado;
}



int imgfs_opendir(const char* ruta, struct fuse_file_info* fi){
	int resultado = 0;
	cantidad_niveles = get_niveles_ruta(ruta);
	nombres_niveles = get_nombres_ruta(cantidad_niveles, ruta);

	//cuando es el directorio root
	if(nombres_niveles == NULL){
		//Se obtiene la estructura directorio
		encabezado_directorio* root = get_directorio_root();
		DEBUG_MENSAJE_UN_ARGUMENTO("Abriendo directorio con ruta: '%s'", root->nombre_directorio);
		fi->fh = (uint64_t) root;
	}else{
		DEBUG_MENSAJE_UN_ARGUMENTO("Buscando directorio para abrir con ruta: '%s'", ruta);
		//se obtiene su referencia
		referencia_archivo* ref = get_referencia_archivo();
		if(ref == NULL)
			resultado = -ENOENT;
		else{
			uint posicion_directorio = get_posicion_bloque_segun_numero(ref->bloques_de_datos[0]);
			DEBUG_MENSAJE_UN_ARGUMENTO("Tiene sus datos en bloque [%d]", ref->bloques_de_datos[0]);
			encabezado_directorio* dir = (encabezado_directorio*) (unidad_almacenamiento_fs + posicion_directorio);
			DEBUG_MENSAJE_UN_ARGUMENTO("Encontrado directorio: [%s]", dir->nombre_directorio);
			fi->fh = (unsigned long) dir;
		}
		desalojar_memoria_nombres_ruta(cantidad_niveles, nombres_niveles);
	}
	return resultado;
}



int imgfs_readdir(const char* ruta, void* buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi){

	(void) ruta;
	(void) offset;
	//se obtiene el directorio
	encabezado_directorio* direc = (encabezado_directorio*) (uintptr_t) fi->fh;
	DEBUG_MENSAJE_UN_ARGUMENTO("Leyendo directorio con nombre: %s", direc->nombre_directorio);
    filler(buffer, ".", NULL, 0);
    filler(buffer, "..", NULL, 0);
    /**---- TODO:darle mas atributos a los archivos, timestampp y demas  ---**/
    int indice_archivos;
    int limite = get_cantidad_maxima_archivos_en_directorio();
    for(indice_archivos = 0; indice_archivos < limite; indice_archivos++){
    	mapeo_archivo* map = ((mapeo_archivo*) get_dato_en_posicion_relativa(direc->p_inicio_mapeos)) + indice_archivos;
    	if(map->estado == 1)
    		filler(buffer, map->nombre, NULL, 0);
//    	mapeo_archivo* map = direc->inicio_mapeos + indice_archivos;
//    	if(map->estado == 1)
//    		filler(buffer, map->nombre, NULL, 0);
    }
	return 0;
}



int imgfs_mkdir(const char* ruta, mode_t modo){
	int resultado = 0;
	DEBUG_MENSAJE_UN_ARGUMENTO("Creando directorio con ruta: [%s]", ruta);
	//se obtiene la referencia para la nueva carpeta
	referencia_archivo* ref_dir = agregar_nuevo_archivo(ruta, DIRECTORIO);
	//se busca el siguiente bloque para guardar el directorio, fantasma si es necesario
	uint num_bloque = get_bloque_fantasma(ref_dir);
	encabezado_directorio* nuevo_dir;

	if(num_bloque != 0){ //es un bloque fantasma
		DEBUG_MENSAJE_UN_ARGUMENTO("Asignandole a la nueva carpeta un bloque fantasma [%d]", num_bloque);
	}else{
		//se obtiene el siguiente bloque de datos, no usado
		num_bloque = super_block->siguiente_bloque_datos;
		DEBUG_MENSAJE_UN_ARGUMENTO("Asginandole a la carpeta un bloque limpio [%d]", num_bloque);
		//se define la posicion en memoria del nuevo directorio
		nuevo_dir = (encabezado_directorio*) (unidad_almacenamiento_fs + get_posicion_bloque_segun_numero(num_bloque));
		super_block->siguiente_bloque_datos--;
		super_block->cantidad_bloques_ocupados++;
	}
	//se asigna el bloque a la referencia del directorio
	ref_dir->bloques_de_datos[0] = num_bloque;
	DEBUG_MENSAJE_UN_ARGUMENTO("Directorio iniciara en bloque [%d]", ref_dir->bloques_de_datos[0]);
	strcpy(nuevo_dir->nombre_directorio, nombre_archivo);
	nuevo_dir->cantidad_actual_archivos = 0;
	nuevo_dir->p_directorio_extra = 0;
	nuevo_dir->p_referencia = get_posicion_relativa_en_unidad((byte*) ref_dir);
	byte* posicion_mapeo = ((byte*) nuevo_dir) + sizeof(encabezado_directorio);
	nuevo_dir->p_inicio_mapeos = get_posicion_relativa_en_unidad(posicion_mapeo);
	DEBUG_MENSAJE_UN_ARGUMENTO("Creado el nuevo directorio [%s]", nuevo_dir->nombre_directorio);
	free(nombre_archivo);
	return resultado;
}



int imgfs_open(const char* ruta, struct fuse_file_info* fi){
	int resultado = 0;
	DEBUG_MENSAJE_UN_ARGUMENTO("Abriendo archivo con ruta: [%s]", ruta);
	cantidad_niveles = get_niveles_ruta(ruta);
	nombres_niveles = get_nombres_ruta(cantidad_niveles, ruta);
	referencia_archivo* ref_archivo_buscado = get_referencia_archivo(cantidad_niveles, nombres_niveles);
	desalojar_memoria_nombres_ruta(cantidad_niveles, nombres_niveles);
	if(ref_archivo_buscado == NULL)
		resultado = -ENOENT;
	else{
		fi->fh = (unsigned long) ref_archivo_buscado;
	}
	return resultado;
}




int imgfs_create(const char* ruta, mode_t modo,  struct fuse_file_info* fi){

	DEBUG_MENSAJE_UN_ARGUMENTO("Dentro de la funcion create, con ruta [%s]", ruta);
	referencia_archivo* nuevo = agregar_nuevo_archivo(ruta, ARCHIVO);
	fi->fh = (uint64_t) nuevo;
	free(nombre_archivo);
	return 0;
}









