/*
 * Server.c
 *
 *  Created on: Oct 12, 2016
 *      Author: francisco
 */

/* INCLUDES */
#include <stdbool.h>            		// bool, true, false
#include "../Constants.h"         		// Mensajes De Error y funciones comunes
#include "../ErrorHandling.h"         	// Mensajes De Error y funciones comunes
#include <stdio.h>              		// printf
#include <stdlib.h>						// exit
#include <string.h>             		// strlen
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>	// Signal

/* DEFINES */
#define MSG_LEN 500
#define STATUS_LEN 100
#define BASIC_PERMISSIONS 0666
#define N 20
#define DEFAULT_CLIENT_AMOUNT 80000

/* ESTRUCTURES */
struct cajero {
    char userCode[30];
    char datetime[30];
    int totalAmount;
};
struct transaction {
    char userCode[30];
	char operation;
    int amount;
    char datetime[30];
};

/* FUNCTION SIGNATURES */
void registrarDepositoEnBitacora();
void registrarRetiroEnBitacora();
static void sigkillHandler(int signo);

/* GLOBAL VARIABLES */
int puertoDeServicio;
FILE * bitacoraRetiro;
FILE * bitacoraRetiro;
int contadorTrasacciones;
int socketDescriptor;
int newSocketDescriptor;
int portno;
char buffer[MSG_LEN];


int main(int argc, char *argv[]) {

	/* LOCAL VARIABLES */
	int i =0;							// Iterador Multi-uso
	int bitPortSwitch = 0;				// Booleanos que indican
	int bitDepSwitch = 0;				// si ya se dio el argumento
	int bitRetSwitch = 0;
	char dirBitacoraDeposito[MSG_LEN];
	char dirBitacoraRetiro[MSG_LEN];
	int clilen;
	int readWriteCode;
	struct sockaddr_in serv_addr, cli_addr;

	/* SIGNALS */
    signal(SIGPIPE, SIG_IGN);			// Manejador de senales para SIGPIPE
	signal(SIGINT, sigkillHandler);		// Manejador de senales para SIGINT
	signal(SIGABRT, sigkillHandler);	// Manejador de senales para SIGINT
	signal(SIGTERM, sigkillHandler); // Manejador de senales para SIGINT

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
				portno=atoi(argv[i + 1]);
				bitPortSwitch = 1;
			}
			/* Caso 2.2: Se recibio el archivo de bitacora de deposito */
			else if ((strcmp(argv[i],"-i")) == 0 && bitDepSwitch == 0)
			{
				strcpy(dirBitacoraDeposito,argv[i + 1]);
				bitacoraRetiro = fopen (dirBitacoraDeposito, "w+");
				bitDepSwitch = 1;
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
		errorAndExit(argNumError);
	}
	/*
		http://www.linuxhowtos.org/C_C++/socket.htm

		The steps involved in establishing a socket on the server side are as
		follows:

		Create a socket with the socket() system call

    	Bind the socket to an address using the bind() system call. For a server
    	socket on the Internet, an address consists of a port number on the
    	host machine.

    	Listen for connections with the listen() system call

    	Accept a connection with the accept() system call. This call typically
    	blocks until a client connects with the server.

    	Send and receive data
	 */
	/* APERTURA DEL SOCKET */
	socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (socketDescriptor < 0)
	{
		errorAndExit("ERROR opening socket");
	}


	/* CONEXION DEL SOCKET */
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(socketDescriptor, (struct sockaddr *) &serv_addr,
				  sizeof(serv_addr)) < 0)
	{
		errorAndExit("ERROR binding socket");
	}
	/* MAIN CICLE */
    while (true)
    {
    	listen(socketDescriptor,5);
		clilen = sizeof(cli_addr);
		/* ACEPTANDO LA CONEXION */
		newSocketDescriptor = accept(socketDescriptor,
				 (struct sockaddr *) &cli_addr,
				 &clilen);
		if (newSocketDescriptor < 0)
		{
			errorAndExit("ERROR on accept");
		}
		/* Vaciamos el buffer */
		bzero(buffer,MSG_LEN);
		/* LEYENDO DEL SOCKET */
		readWriteCode = read(newSocketDescriptor,buffer,255);
		if (readWriteCode < 0)
		{
			errorAndExit("ERROR reading from socket");
		}
		printf("Here is the message: %s\n",buffer);
		/* ESCRIBIENDO AL SOCKET */
		readWriteCode = write(newSocketDescriptor,"I got your message",18);
		if (readWriteCode < 0)
		{
			errorAndExit("ERROR writing to socket");
		}

    }
	close(newSocketDescriptor);
	close(socketDescriptor);
	return 0;
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

/*
 * Function:  sigkillHandler
 * --------------------
 *  Manejador de señales por si el servidor se cierra inesperadamente
 *
 *  signo:
 *
 *  returns: void
 */
static void sigkillHandler(int signo)
{
	printf("Saliendo...\n");
	close(newSocketDescriptor);
	close(socketDescriptor);
    exit(0);
}
