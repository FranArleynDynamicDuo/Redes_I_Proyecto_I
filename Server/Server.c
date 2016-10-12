/*
 * Server.c
 *
 *  Created on: Oct 12, 2016
 *      Author: francisco
 */

/* INCLUDES */
#include <stdbool.h>            // bool, true, false
#include "../Constants.h"         	// Mensajes De Error y funciones comunes
#include "../ErrorHandling.h"         	// Mensajes De Error y funciones comunes
#include <stdio.h>              // printf
#include <stdlib.h>				// exit
#include <string.h>             // strlen

/* DEFINES */
#define MSG_LEN 500
#define STATUS_LEN 100
#define BASIC_PERMISSIONS 0666
#define N 20

/* ESTRUCTURES */
struct transaction {
    char userCode[30];
	char operation;
    int amount;
    char datetime[30];
};

/* FUNCTION SIGNATURES */
void registrarDepositoEnBitacora();
void registrarRetiroEnBitacora();

/* GLOBAL VARIABLES */
char puertoDeServicio[MSG_LEN];
FILE * bitacoraRetiro;
FILE * bitacoraRetiro;
int contadorTrasacciones;


int main(int argc, char *argv[]) {

	/* LOCAL VARIABLES */
	int i =0;				// Iterador Multi-uso
	int bitPortSwitch = 0;	// Booleanos que indican si ya se dio el argumento
	int bitDepSwitch = 0;
	int bitRetSwitch = 0;
	char dirBitacoraDeposito[MSG_LEN];
	char dirBitacoraRetiro[MSG_LEN];

	/* SIGNALS */

	/* ARGUMENT MANAGEMENT */
	/* Caso 0: Se recibio los argumentos */
	if (argc == 7)
	{
		/* Recorremos la lista de argumentos */
		for (i=1; i<argc; i = i + 2)
		{
			/* Caso 2.1: Se recibio el numero de puerto */
			if ((strcmp(argv[i],"-l")) == 0 && bitPortSwitch == 0)
			{
				strcpy(puertoDeServicio,argv[i + 1]);
				bitPortSwitch = 1;
			}
			/* Caso 2.2: Se recibio el archivo de bitacora de deposito */
			else if ((strcmp(argv[i],"-i")) == 0 && bitDepSwitch == 0)
			{
				strcpy(dirBitacoraDeposito,argv[i + 1]);
				bitacoraRetiro = fopen (dirBitacoraDeposito, "w+");
				bitRetSwitch = 1;
			}
			/* Caso 2.3: Se recibio el archivo de bitacora de retiro */
			else if ((strcmp(argv[i],"-o")) == 0 && bitRetSwitch == 0)
			{
				strcpy(dirBitacoraRetiro,argv[i + 1]);
				bitacoraRetiro = fopen (dirBitacoraRetiro, "w+");
				bitRetSwitch = 1;
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
	/* MAIN CICLE */
    while (true)
    {

    }
}

/* FUNCTION DECLARATION */
/*
 * Function:  registrarDepositoEnBitacora()
 * --------------------
 *  Escribe el registro de un deposito hecho en la bitacora
 *
 *  returns: void
 */
void registrarDepositoEnBitacora()
{
}

/*
 * Function:  registrarRetiroEnBitacora()
 * --------------------
 *  Escribe el registro de un retiro hecho en la bitacora
 *
 *  returns: void
 */
void registrarRetiroEnBitacora()
{
}
