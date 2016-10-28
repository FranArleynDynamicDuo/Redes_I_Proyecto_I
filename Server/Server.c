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
#define CODE_LEN 50
#define STATUS_LEN 100
#define BASIC_PERMISSIONS 0666
#define N 20
#define DEFAULT_CLIENT_AMOUNT 80000
#define MAX_CAJEROS 3
#define MAX_USUARIOS 10

/* ESTRUCTURES */
struct transaction {
	char userCode[CODE_LEN];
	char operation;
	int amount;
	char date[CODE_LEN];
	char time[CODE_LEN];
	int cajero;
};
struct user {
	char userCode[CODE_LEN];
	int retiros;
};
/* Estructura que guarda los argumentos a ser utilizados por los hilos */
struct thread_data
{
	int  thread_id;
	char userCode[CODE_LEN];
	char operation;
	int amount;
	char date[CODE_LEN];
	char time[CODE_LEN];
	int cajero;
};

/* FUNCTION SIGNATURES */
static void sigkillHandler(int signo);
void retiro(int cajero,char idUsuario[],int monto);
void deposito(int cajero,char idUsuario[],int monto);
void inicializarBitacoraDeposito();
void inicializarBitacoraRetiro();
void registrarDepositoEnBitacora(struct transaction trans);
void registrarRetiroEnBitacora(struct transaction trans);
void imprimeTicket(struct transaction trans);
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
struct user usuarios[MAX_USUARIOS];
struct sockaddr_in cajeros_addr[MAX_CAJEROS];
int balanceCajeros[MAX_CAJEROS];
int numeroCajeros=0;
int numeroUsuarios=0;

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
	char userId[CODE_LEN];
	char *token;
	int rc;
	pthread_t hilos[MAX_CAJEROS];				// Lista de hilos
	struct thread_data thread_data_array[MAX_CAJEROS];		// Variable que guarda los argumentos a ser utilizados por los hilos


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
		/* REGISTRO DE CAJERO */
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

		/* MANEJO DE MENSAJE */
		thread_data_array[cajeroSeleccionado].cajero=cajeroSeleccionado;

		/* Operacion */
		token = strtok(buffer, "-");
		thread_data_array[cajeroSeleccionado].operation=token[0];
		/* Codigo De Usuario */
		token = strtok(0, "-");
		strcpy(thread_data_array[cajeroSeleccionado].userCode,token);
		/* Monto */
		token = strtok(0, "-");
		thread_data_array[cajeroSeleccionado].amount=atoi(token);

		/* REGISTRO DE USUARIO */

		/* Buscamos al usuario */
		encontrado=false;
		for (i=0;i<MAX_USUARIOS;i++)
		{
			if(strcmp(thread_data_array[cajeroSeleccionado].userCode,usuarios[i].userCode)==0)
			{
				encontrado=true;
				break;
			}
		}
		/* Si no esta registrado lo registramos */
		if (!encontrado)
		{
			strcpy(usuarios[numeroUsuarios].userCode,thread_data_array[cajeroSeleccionado].userCode);
			usuarios[numeroUsuarios].retiros=0;
			numeroUsuarios+=1;
		}

		rc = pthread_create(&hilos[cajeroSeleccionado], NULL, efectuarOperacion, (void *) &thread_data_array[cajeroSeleccionado]);
		if (rc)
		{
			errorAndExit(pthreadCreateError);
		}
	}
	close(newSocketDescriptor);
	close(socketDescriptor);
	return 0;
}

void inicializarBitacoraDeposito()
{
	bitacoraDeposito = fopen (dirBitacoraDeposito, "w+");
	fprintf(bitacoraDeposito,"-------------------------------------------- Bitacora de Depositos --------------------------------------------\n");
	fprintf(bitacoraDeposito,"Cajero           Fecha           Hora       Codigo    Operacion      Monto      Total Disponible\n");
	fprintf(bitacoraDeposito,"-------------------------------------------------------------------------------------------------------------\n");
	fprintf(bitacoraDeposito,"\n");
	fclose(bitacoraDeposito);
}

void inicializarBitacoraRetiro()
{
	bitacoraRetiro = fopen (dirBitacoraRetiro, "w+");
	fprintf(bitacoraRetiro,"-------------------------------------------- Bitacora de Retiros --------------------------------------------\n");
	fprintf(bitacoraRetiro,"Cajero           Fecha           Hora       Codigo    Operacion      Monto      Total Disponible\n");
	fprintf(bitacoraRetiro,"-------------------------------------------------------------------------------------------------------------\n");
	fprintf(bitacoraRetiro,"\n");
	fclose(bitacoraRetiro);
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
	int readWriteCode;
	int i=0;

	retiro.amount=monto;
	retiro.operation='r';
	strcpy(retiro.userCode,"");
	strcpy(retiro.userCode,idUsuario);
	retiro.cajero=cajero;

	if 	(retiro.amount <= balanceCajeros[cajero])
	{
		/* Buscamos al usuario */
		for (i=0;i<MAX_USUARIOS;i++)
		{
			if(strcmp(retiro.userCode,usuarios[i].userCode)==0)
			{
				break;
			}
		}
		/* Si el usuario tiene 3 retiros o mas, no puede retirar mas por la sesion */
		if (usuarios[i].retiros>=3)
		{
			readWriteCode = write(newSocketDescriptor,"Ya acabo su cupo de retiros por hoy\n",MSG_LEN);
			if (readWriteCode < 0)
			{
				errorAndExit("ERROR writing to socket");
			}
		}
		else
		{
			balanceCajeros[cajero] = balanceCajeros[cajero] - retiro.amount;
			printf("Valor del retiro: %d\n", retiro.amount);
			printf("Valor del Total Disponible: %d\n",balanceCajeros[cajero]);
			time_t tiempo = time(0);
			struct tm *tlocal = localtime(&tiempo);
			strftime(retiro.date, 50, "%d/%m/%y", tlocal);
			strftime(retiro.time, 50, "%H:%M:%S", tlocal);
			usuarios[i].retiros+=1;
			imprimeTicket(retiro);
			registrarRetiroEnBitacora(retiro);
		}

	}
	else
	{
		readWriteCode = write(newSocketDescriptor,"Dinero No Disponible\n",MSG_LEN);
		if (readWriteCode < 0)
		{
			errorAndExit("ERROR writing to socket");
		}
	}
}

void deposito(int cajero,char idUsuario[],int monto)
{
	struct transaction deposito;
	deposito.amount=monto;
	deposito.operation='d';
	strcpy(deposito.userCode,"");
	strcpy(deposito.userCode,idUsuario);

	balanceCajeros[cajero] = balanceCajeros[cajero] + deposito.amount;

	printf("Valor del deposito: %d\n", deposito.amount);
	printf("Valor del Total Disponible: %d\n",balanceCajeros[cajero]);

	time_t tiempo = time(0);
	struct tm *tlocal = localtime(&tiempo);
	strftime(deposito.date, 50, "%d/%m/%y", tlocal);
	strftime(deposito.time, 50, "%H:%M:%S", tlocal);

	imprimeTicket(deposito);
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
	fprintf(bitacoraDeposito,"  %d             %s       %s     %s     Deposito          %d            %d\n",
			trans.cajero,
			trans.date,
			trans.time,
			trans.userCode,
			trans.amount,
			balanceCajeros[trans.cajero]);
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
	bitacoraRetiro = fopen (dirBitacoraRetiro, "a");
	fprintf(bitacoraRetiro,"  %d             %s       %s     %s     Retiro          %d            %d\n",
			trans.cajero,
			trans.date,
			trans.time,
			trans.userCode,
			trans.amount,
			balanceCajeros[trans.cajero]);
	fclose(bitacoraRetiro);
}
/*
 * Function:  imprimeTicket()
 * --------------------
 *  Imprime el ticket
 *
 *  returns: void
 */
void imprimeTicket(struct transaction trans)
{
	char ticket[700];
	int readWriteCode;

	printf("\n");
	printf("Se Imprime el Ticket del Usuario:\n");
	sprintf(ticket, "Fecha: %s\n	Hora: %s\n	Codigo: %s\n", trans.date, trans.time,trans.userCode);

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
	struct thread_data *dataHilo;			// Variable temporal para obtener los argumentos de la funcion
	/* Obtenemos los argumentos del hilo */
	dataHilo = (struct thread_data *) threadarg;
	if (dataHilo->operation=='d')
	{
		deposito(dataHilo->cajero,dataHilo->userCode,dataHilo->amount);
	}
	else if (dataHilo->operation=='r')
	{
		retiro(dataHilo->cajero,dataHilo->userCode,dataHilo->amount);
	}
	return (void *) 0;
}

