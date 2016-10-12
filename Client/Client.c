/*
 * Client.c
 *
 *  Created on: Oct 12, 2016
 *      Author: francisco
 */

/* INCLUDES */
#include <stdbool.h>					// bool, true, false
#include "../Constants.h"         	// Mensajes De Error y funciones comunes
#include "../ErrorHandling.h"         	// Mensajes De Error y funciones comunes

/* DEFINES */
#define MSG_LEN 500
#define STATUS_LEN 100
#define BASIC_PERMISSIONS 0666
#define N 20

/* ESTRUCTURES */

/* FUNCTION SIGNATURES */

/* GLOBAL VARIABLES */
int totalDisponible = 80000;
char operation;
char serverPort[30];
char userId[30];
char atentionModule[30];

int main(int argc, char *argv[]) {
	wait(1);

	/* LOCAL VARIABLES */
	int i =0;				// Iterador Multi-uso
	int serverPortSwitch = 0;	// Booleanos que indican si ya se dio el argumento
	int userIdSwitch = 0;
	int atmoduleSwitch = 0;
	int operationSwitch = 0;

	/* SIGNALS */

	/* ARGUMENT MANAGEMENT */
	/* Caso 0: Se recibio los argumentos */
	if (argc == 9)
	{
		/* Recorremos la lista de argumentos */
		for (i=1; i<argc; i = i + 2)
		{
			/* Caso 2.1: Se recibio el numero de puerto */
			if ((strcmp(argv[i],"-d")) == 0 && atmoduleSwitch == 0)
			{
				strcpy(atentionModule,argv[i + 1]);
				atmoduleSwitch = 1;
			}
			/* Caso 2.2: Se recibio el archivo de bitacora de deposito */
			else if ((strcmp(argv[i],"-i")) == 0 && userIdSwitch == 0)
			{
				strcpy(userId,argv[i + 1]);
				userIdSwitch = 1;
			}
			/* Caso 2.3: Se recibio el archivo de bitacora de retiro */
			else if ((strcmp(argv[i],"-p")) == 0 && serverPortSwitch == 0)
			{
				strcpy(serverPort,argv[i + 1]);
				serverPortSwitch = 1;
			}
			/* Caso 2.3: Se recibio el archivo de bitacora de retiro */
			else if ((strcmp(argv[i],"-c")) == 0 && operationSwitch == 0)
			{
				strcpy(operation,argv[i + 1]);
				operationSwitch = 1;
			}
			/* Caso 2.4: Se recibieron argumentos en un formato invalido */
			else
			{
				errorAndExit(invOrdError);
			}
		}
	}
	else
	{
		printf("%s", argNumError);
		exit(0);

	}
}

/* FUNCTION DECLARATION */
