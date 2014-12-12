/*
 * debug.h
 *
 *  Created on: 25/11/2014
 *      Author: mauriciopaz
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdio.h>
#define IMPRESION_HABILITADA

#ifdef IMPRESION_HABILITADA
	#define DEBUG_MENSAJE( mensaje ) puts( "******Debug: " mensaje )
	#define DEBUG_MENSAJE_UN_ARGUMENTO( mensaje, arg) printf( "******Debug: " mensaje "\n", arg)
	#define DEBUG_MENSAJE_DOS_ARGUMENTOS( mensaje, arg1, arg2 ) printf( "******Debug: " mensaje "\n", arg1, arg2)
	#define DEBUG_MENSAJE_TRES_ARGUMENTOS( mensaje, arg1, arg2, arg3) printf( "******Debug: " mensaje "\n", arg1, arg2, arg3)
#else
	#define DEBUG_MENSAJE( mensaje )
	#define DEBUG_MENSAJE_UN_ARGUMENTO( mensaje, argumento)
	#define DEBUG_MENSAJE_DOS_ARGUMENTOS( mensaje, arg1, arg2 )
	#define DEBUG_MENSAJE_TRES_ARGUMENTOS( mensaje, arg1, arg2, arg3)
#endif

#define DEBUG_ERORR( mensaje ) fprintf( stderr, "******Error: " mensaje "\n")
#define DEBUG_ERORR_UN_ARGUMENTO( mensaje, arg ) fprintf( stderr, "******Error: " mensaje "\n", arg)

#endif /* DEBUG_H_ */
