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
#include <sys/socket.h>
#include <string.h>             		// strlen

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
int socketDescriptor;

int main(int argc, char *argv[]) {

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
				operation=argv[i + 1];
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
		errorAndExit(argNumError);
	}
	/*
		http://www.linuxhowtos.org/C_C++/socket.htm

		The steps involved in establishing a socket on the client side are
		as follows:

    	Create a socket with the socket() system call

    	Connect the socket to the address of the server using the connect()
    	system call

    	Send and receive data. There are a number of ways to do this,
    	but the simplest is to use the read() and write() system calls.
	 */
	/* APERTURA DEL SOCKET */
	socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (socketDescriptor < 0)
	{
		errorAndExit("ERROR opening socket");
	}
}

/* FUNCTION DECLARATION */
