/*
 * ErrorHandling.c
 *
 *	Archivo que contenera funciones para imprimir mensajes de error y funciones
 *	que sirvan como manejadores de excepciones
 *
 *  Created on: Oct 12, 2016
 *      Author: francisco
 */

/* INCLUDES */
#include <stdio.h>              // printf
#include <stdlib.h>				// exit

/* DEFINES */
#define MSG_LEN 500
#define STATUS_LEN 100
#define BASIC_PERMISSIONS 0666
#define N 20

/* ESTRUCTURES */

/* FUNCTION SIGNATURES */

/* GLOBAL VARIABLES */

/*
 * Function:  errorAndExit
 * --------------------
 *  Imprime un error y sale del programa.
 *
 *	errorMessage: el mensaje de error a utilizar
 *
 *  returns: void
 */
void errorAndExit(const char* errorMessage)

{
	perror(errorMessage);
	printf("\n");
	exit(1);
}
