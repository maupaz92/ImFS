/*
 * main.c
 *
 *  Created on: 25/11/2014
 *      Author: mauriciopaz
 */

#include "funciones_fuse.h"
#include "debug.h"

static struct fuse_operations imgfs_operaciones = {
		.getattr		= 	imgfs_getattr,
		.opendir		=	imgfs_opendir,
		.readdir		=	imgfs_readdir,
//		.open 			=	imgfs_open,
//		.read			=	imgfs_read,
//		.create			= 	imgfs_create,
		.mkdir			=	imgfs_mkdir,
//		.write			=	imgfs_write,
//		.destroy		=	imgfs_destroy,
//		.truncate		=	imgfs_truncate,
//		.unlink			=	imgfs_unlink
};


int main(int argc, char** argumentos){

	unidad_almacenamiento_fs = (byte*)calloc(TAMANIO_BYTES_SISTEMA_ARCHIVOS, sizeof(byte));
	if(unidad_almacenamiento_fs == NULL){
		DEBUG_ERORR("No se pudo obtener memoria para el sistema de archivos, en funcion 'asignar_espacio_sistema_archivos'");
		exit(EXIT_FAILURE);
	}

	crear_sistema_archivos(TAMANIO_BYTES_SISTEMA_ARCHIVOS);

	return fuse_main(argc, argumentos, &(imgfs_operaciones), NULL);
}







