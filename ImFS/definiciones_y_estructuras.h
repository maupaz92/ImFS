/*
 * definiciones_y_estructuras.h
 *
 *  Created on: 25/11/2014
 *      Author: mauriciopaz
 */

#ifndef DEFINICIONES_Y_ESTRUCTURAS_H_
#define DEFINICIONES_Y_ESTRUCTURAS_H_

#include <stdlib.h>

typedef struct super_bloque					super_bloque;
typedef struct referencia_archivo 			referencia_archivo;
typedef struct encabezado_directorio 		encabezado_directorio;
typedef struct encabezado_referencias 		encabezado_referencias;
typedef struct mapeo_archivo_identificador 	mapeo_archivo;

typedef unsigned char byte;
typedef unsigned long posicion_memoria;
typedef unsigned int uint;
typedef enum {FANTASMA, ARCHIVO, DIRECTORIO} tipo_archivo;


#define TAMANIO_BLOQUE_BYTES 				512
#define BLOQUES_DATOS_POR_REFERENCIA 		2
#define LONGITUD_MAXIMA_NOMBRE_ARCHIVO 		20
#define TAMANIO_BYTES_SISTEMA_ARCHIVOS 		20*1024
#define CANTIDAD_ARCHIVOS_POR_DIRECTORIO 	10


struct super_bloque{
	//bytes disponibles para el FS
	uint largo_total_unidad;
	uint cantidad_bloques;
	uint cantidad_bloques_ocupados;
	// numero del siguiente bloque para agregar mas referencias
	uint siguiente_bloque_referencias;
	//numero para el siguiente bloque de datos para asignar datos
	uint siguiente_bloque_datos;
	uint espacio_disponible;
	encabezado_referencias* inicio_referencias_archivos;
	posicion_memoria p_inicio_referencias_archivos;
};


struct encabezado_referencias{
	uint cantidad_bloques_fantasma;
	byte cantidad_actual_referencias;
	byte cantidad_referencias_fantasma;
	referencia_archivo* referencia_inicial;
	posicion_memoria p_referencia_inicial;
	encabezado_referencias* siguiente;
	posicion_memoria p_siguiente;
};


struct referencia_archivo {
	tipo_archivo tipo_archivo;
	byte bloques_totales;
	byte bloques_fantasma;
	byte bloques_asignados;
	encabezado_referencias* encabezado;
	posicion_memoria p_encabezado_contenedor;
	uint tamanio;
	// posiciones de los bloques que contienene la informacion del archivo
	uint bloques_de_datos[BLOQUES_DATOS_POR_REFERENCIA];
	referencia_archivo* referencia_extra;
	posicion_memoria p_referencia_extra;
};


/**
 *  TODO: pensar mejor el manejo del encabezado de directorios, tiene un nombre
 *  pero que pasa si se elimina el directorio, el nombre queda flotando? o que?
 */

struct encabezado_directorio{
	char nombre_directorio[LONGITUD_MAXIMA_NOMBRE_ARCHIVO + 1];
	byte cantidad_actual_archivos;
	referencia_archivo* referencia;
	posicion_memoria p_referencia;
	mapeo_archivo* inicio_mapeos;
	posicion_memoria p_inicio_mapeos;
	encabezado_directorio* directorio_extra;
	posicion_memoria p_directorio_extra;
};


struct mapeo_archivo_identificador{
	char nombre[LONGITUD_MAXIMA_NOMBRE_ARCHIVO + 1];
	byte estado;
	referencia_archivo* referencia;
	posicion_memoria p_referencia;
};


super_bloque* super_block;
byte* unidad_almacenamiento_fs;
byte* unidad_almacenamiento_completa;











#endif /* DEFINICIONES_Y_ESTRUCTURAS_H_ */
