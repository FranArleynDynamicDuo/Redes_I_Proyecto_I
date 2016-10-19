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
#include <time.h>
#include <pthread.h>

/* DEFINES */
#define STATUS_LEN 100
#define BASIC_PERMISSIONS 0666
#define N 20
#define DEFAULT_CLIENT_AMOUNT 80000
#define MAX_CAJEROS 3
#define MAXTHREADS 6

/* ESTRUCTURES */
struct transaction {
	char userCode[30];
	char operation;
	int amount;
	char date[30];
	char time[30];
	int cajero;
};
/* Estructura que guarda los argumentos a ser utilizados por los hilos */
struct thread_data
{
	int  thread_id;
	char userCode[30];
	char operation;
	int amount;
	char date[30];
	char time[30];
	int cajero;
};

/* FUNCTION SIGNATURES */
static void sigkillHandler(int signo);
void retiros();
void deposito();
void inicializarBitacoraDeposito();
void inicializarBitacoraRetiro();
void registrarDepositoEnBitacora();
void registrarRetiroEnBitacora();
void imprimeTicket();
void *efectuarOperacion(void *threadarg);

/* GLOBAL VARIABLES */
char dirBitacoraDeposito[MSG_LEN];
char dirBitacoraRetiro[MSG_LEN];
int puertoDeServicio;
FILE * bitacoraDeposito;
FILE * bitacoraRetiro;
int contadorTrasacciones;
int socketDescriptor;
int newSocketDescriptor;
int portno;
char buffer[MSG_LEN];
struct sockaddr_in cajeros_addr[MAX_CAJEROS];
int balanceCajeros[MAX_CAJEROS];
int numeroCajeros=0;
struct thread_data thread_data_array[MAX_CAJEROS];		// Variable que guarda los argumentos a ser utilizados por los hilos

int main(int argc, char *argv[]) {

	/* LOCAL VARIABLES */
	int i =0;							// Iterador Multi-uso
	int bitPortSwitch = 0;				// Booleanos que indican
	int bitDepSwitch = 0;				// si ya se dio el argumento
	int bitRetSwitch = 0;

	int clilen;
	int readWriteCode;
	struct sockaddr_in serv_addr, cli_addr;
	int pid;
	int encontrado;
	int cajeroSeleccionado;
	int trabajando;
	int libres;
	char operation;
	int amount;
	char userId[30];
	char *token;

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
				inicializarBitacoraDeposito();
				bitDepSwitch = 1;
			}
			/* Caso 2.3: Se recibio el archivo de bitacora de retiro */
			else if ((strcmp(argv[i],"-o")) == 0 && bitRetSwitch == 0)
			{
				strcpy(dirBitacoraRetiro,argv[i + 1]);
				inicializarBitacoraRetiro();
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
	listen(socketDescriptor,5);
	clilen = sizeof(cli_addr);

	/* MAIN CICLE */
	while (true)
	{

		/* ACEPTANDO LA CONEXION */
		newSocketDescriptor = accept(socketDescriptor,
				(struct sockaddr *) &cli_addr,
				&clilen);
		if (newSocketDescriptor < 0)
		{
			errorAndExit("ERROR on accept");
		}

		/* Verificamos si el cajero ya esta registrado */
		encontrado=false;
		for (i=0;i<MAX_CAJEROS;i++)
		{
			if (cajeros_addr[i].sin_addr.s_addr==cli_addr.sin_addr.s_addr)
			{
				encontrado=true;
				cajeroSeleccionado=i;
				break;
			}
		}

		/* Si no esta registrado lo registramos */
		if (!encontrado)
		{
			cajeros_addr[numeroCajeros]=cli_addr;
			balanceCajeros[numeroCajeros]=DEFAULT_CLIENT_AMOUNT;
			cajeroSeleccionado=numeroCajeros;
			numeroCajeros+=1;
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

		thread_data_array[cajeroSeleccionado].cajero=cajeroSeleccionado;

		/* Operacion */
		token = strtok(buffer, "-");
		thread_data_array[cajeroSeleccionado].operation=token[0];
		/* Codigo De Usuario */
		token = strtok(0, "-");
		bzero(thread_data_array[cajeroSeleccionado].userCode,30);
		strcpy(thread_data_array[cajeroSeleccionado].userCode,userId);
		/* Monto */
		token = strtok(0, "-");
		thread_data_array[cajeroSeleccionado].amount=atoi(token);

		/* Create child process */
		pid = fork();

		if (pid < 0)
		{
			perror("ERROR on fork");
			exit(1);
		}

		if (pid == 0)
		{
			/* This is the client process */
			close(socketDescriptor);


			/* ESCRIBIENDO AL SOCKET */
			if (readWriteCode < 0)
			{
				errorAndExit("ERROR writing to socket");
			}
			exit(0);
		}
		else
		{
			close(newSocketDescriptor);
		}
	}
	close(newSocketDescriptor);
	close(socketDescriptor);
	return 0;
}

void inicializarBitacoraDeposito()
{
	bitacoraDeposito = fopen (dirBitacoraDeposito, "w+");
	fputs( "---------------------Bitacora de Depositos ------------------\n", bitacoraDeposito );
	fputs( "\n", bitacoraDeposito );
	fputs( "Cajero      Fecha      Hora      Codigo   Operacion   Total Disponible\n", bitacoraDeposito );
	fputs( "-------------------------------------------------------------\n", bitacoraDeposito );
	fputs( "\n", bitacoraDeposito );
}

void inicializarBitacoraRetiro()
{
	bitacoraRetiro = fopen (dirBitacoraRetiro, "w+");
	fputs( "---------------------Bitacora de Retiros --------------------\n", bitacoraRetiro );
	fputs( "\n", bitacoraRetiro );
	fputs( "Cajero      Fecha      Hora      Codigo   Operacion   Total Disponible\n", bitacoraRetiro );
	fputs( "-------------------------------------------------------------\n", bitacoraRetiro );
	fputs( "\n", bitacoraRetiro );
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

void retiro(int cajero,char idUsuario[],int monto)
{
	struct transaction retiro;
	retiro.amount=monto;

	if 	(retiro.amount <= balanceCajeros[cajero])
	{
		if ( retiro.amount <= 3000 )
		{
			balanceCajeros[cajero] = balanceCajeros[cajero] - retiro.amount;
			printf("Valor del retiro: %d\n", retiro.amount);
			printf("Valor del Total Disponible: %d\n",balanceCajeros[cajero]);
			strcpy(retiro.userCode,idUsuario);
			retiro.cajero=cajero;
			retiro.operation='d';
			time_t tiempo = time(0);
			struct tm *tlocal = localtime(&tiempo);
			strftime(retiro.date, 50, "%d/%m/%y", tlocal);
			strftime(retiro.time, 50, "%H:%M:%S", tlocal);

			imprimeTicket(&idUsuario, retiro);
			registrarRetiroEnBitacora(retiro);

		}
		else
		{
			printf("El monto maximo de retiro es 3000\n");
		}
	}
	else
	{
		printf("Dinero No Disponible\n");
	}
}

void deposito(int cajero,char idUsuario[],int monto)
{
	struct transaction deposito;
	deposito.amount=monto;
	balanceCajeros[cajero] = balanceCajeros[cajero] + deposito.amount;

	printf("Valor del deposito: %d\n", deposito.amount);
	printf("Valor del Total Disponible: %d\n",balanceCajeros[cajero]);

	strcpy(deposito.userCode,idUsuario);
	deposito.cajero=cajero;
	deposito.operation='d';
	time_t tiempo = time(0);
	struct tm *tlocal = localtime(&tiempo);
	strftime(deposito.date, 50, "%d/%m/%y", tlocal);
	strftime(deposito.time, 50, "%H:%M:%S", tlocal);

	imprimeTicket(&idUsuario, deposito);
	registrarDepositoEnBitacora(deposito);
}

/* FUNCTION DECLARATION */
/*
 * Function:  registrarDepositoEnBitacora()
 * --------------------
 *  Escribe el registro de un deposito hecho en la bitacora
 *
 *  returns: void
 */
void registrarDepositoEnBitacora(struct transaction trans)
{
	bitacoraDeposito = fopen (dirBitacoraDeposito, "a");
	fprintf(bitacoraDeposito,"%d   %s   %s   %s   Deposito        %d\n",
			trans.cajero,
			trans.date,
			trans.time,
			trans.userCode,
			trans.amount);
	fclose(bitacoraDeposito);
}
/*
 * Function:  registrarRetiroEnBitacora()
 * --------------------
 *  Escribe el registro de un retiro hecho en la bitacora
 *
 *  returns: void
 */
void registrarRetiroEnBitacora(struct transaction trans)
{
	bitacoraDeposito = fopen (dirBitacoraRetiro, "a");
	fprintf(bitacoraRetiro,"%d   %s   %s   %s   Retiro        %d\n",
			trans.cajero,
			trans.date,
			trans.time,
			trans.userCode,
			trans.amount);
	fclose(bitacoraRetiro);
}
/*
 * Function:  imprimeTicket()
 * --------------------
 *  Imprime el ticket
 *
 *  returns: void
 */
void imprimeTicket( int *idUsuario, struct transaction trans)
{
	char ticket[700];
	int readWriteCode;

	printf("\n");
	printf("Se Imprime el Ticket del Usuario:\n");
	sprintf(ticket, "Fecha: %s\n	Hora: %s\n	Codigo: %d\n", trans.date, trans.time,*idUsuario);

	if (trans.operation=='d')
	{
		strcat(ticket, "	Operacion: Deposito\n");
	}
	else if (trans.operation=='r')
	{
		strcat(ticket, "	Operacion: Retiro\n");
	}

	readWriteCode = write(newSocketDescriptor,ticket,MSG_LEN);

}

/*
 * Function:  efectuarOperacion(void *threadarg)
 * --------------------
 *
 *
 *
 *  returns: -1 si tiene un error y 0 si se ejecuto correctamente
 */
void *efectuarOperacion(void *threadarg)
{
	close(socketDescriptor);
	struct thread_data *dataHilo;			// Variable temporal para obtener los argumentos de la funcion
	/* Obtenemos los argumentos del hilo */
	dataHilo = (struct thread_data *) threadarg;
	if (dataHilo->operation=='d')
	{
		retiro(dataHilo->cajero,dataHilo->userCode,dataHilo->amount);
	}
	else if (dataHilo->operation=='r')
	{
		deposito(dataHilo->cajero,dataHilo->userCode,dataHilo->amount);
	}
	return (void *) 0;
}

