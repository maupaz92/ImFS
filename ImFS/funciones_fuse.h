/*
 * funciones_fuse.h
 *
 *  Created on: 30/11/2014
 *      Author: mauriciopaz
 */

#ifndef FUNCIONES_FUSE_H_
#define FUNCIONES_FUSE_H_

#include "definiciones_y_estructuras.h"

#define FUSE_USE_VERSION 26

#include <fuse.h>

posicion_memoria referencia_inicial_memoria;
int cantidad_niveles;
char** nombres_niveles;
char* nombre_archivo;


int get_cantidad_maxima_archivos_en_directorio();
uint get_bloque_fantasma(referencia_archivo* referencia);
uint get_posicion_bloque_segun_numero(uint numero_bloque);
void crear_sistema_archivos(int tamanio_unidad);
void desalojar_memoria_nombres_ruta(int niveles, char** nombres);
void agregar_referencia_a_directorio(referencia_archivo* referencia, char* nombre, encabezado_directorio* direct);
posicion_memoria get_posicion_relativa_en_unidad(byte* puntero);
byte* get_dato_en_posicion_relativa(posicion_memoria posicion_relativa);
encabezado_directorio* get_directorio_root();
encabezado_referencias* get_encabezado_con_espacio(int* posicion_nueva_ref);
encabezado_directorio* get_directorio_para_archivo(int niveles_ruta, char** nombres_niveles);
referencia_archivo* get_referencia_root();
referencia_archivo* get_referencia_archivo();
referencia_archivo* agregar_nuevo_archivo(const char* ruta, tipo_archivo tipo);
char* get_nombre_archivo(int niveles_ruta, char** nombres_niveles);


int imgfs_getattr(const char* ruta, struct stat* stbuf);
int imgfs_opendir(const char* ruta, struct fuse_file_info* fi);
int imgfs_readdir(const char* ruta, void* buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi);
int imgfs_mkdir(const char* ruta, mode_t modo);
int imgfs_open(const char* ruta, struct fuse_file_info* fi);
int imgfs_create(const char* ruta, mode_t modo,  struct fuse_file_info* fi);



//static struct fuse_operations imgfs_operaciones = {
//		.getattr		= 	imgfs_getattr,
//		.opendir		=	imgfs_opendir,
//		.readdir		=	imgfs_readdir,
////		.open 			=	imgfs_open,
////		.read			=	imgfs_read,
////		.create			= 	imgfs_create,
//		.mkdir			=	imgfs_mkdir,
////		.write			=	imgfs_write,
////		.destroy		=	imgfs_destroy,
////		.truncate		=	imgfs_truncate,
////		.unlink			=	imgfs_unlink
//};






#endif /* FUNCIONES_FUSE_H_ */
