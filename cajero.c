/*
 * Cajero.c
 *
 *  Created on: Oct 17, 2016
 *      Author: Arleyn
 */

/* INCLUDES */
#include <stdio.h>
#include <time.h>


/* DEFINES */


/* ESTRUCTURES */

struct cajero {
	int totalAmount;
};

struct transaction {
	char userCode[30];
	char operation;
	int amount;
	char date[30];
	char time[30];
};

/* FUNCTION SIGNATURES */
void retiro();
void deposito();
void registrarDepositoEnBitacora();
void registrarRetiroEnBitacora();
void imprimeTicket();


/* GLOBAL VARIABLES */

// bitacoraDeposito = fopen (dirBitacoraDeposito, "w+");
// bitacoraRetiro = fopen (dirBitacoraRetiro, "w+");

FILE * bitacoraDeposito;
FILE * bitacoraRetiro;
int totalDisponible = 80000;
char operation[1] = "i";
int IdUsuario;
char fecha[50];
char hora[50];

int main(int argc, char *argv[]) {

	int i = 10;					// Iterador Multi-
	int j = 0;

	bitacoraDeposito = fopen ("deposito.txt", "w+");
	fputs( "---------------------Bitacora de Depositos ------------------\n", bitacoraDeposito );
	fputs( "\n", bitacoraDeposito );
	fputs( "  Fecha      Hora      Codigo   Operacion   Total Disponible\n", bitacoraDeposito );
	fputs( "-------------------------------------------------------------\n", bitacoraDeposito );
	fputs( "\n", bitacoraDeposito );
	fclose(bitacoraDeposito);

	bitacoraRetiro = fopen ("retiros.txt", "w+");
	fputs( "---------------------Bitacora de Retiros ------------------\n", bitacoraRetiro );
	fputs( "\n", bitacoraRetiro );
	fputs( "  Fecha      Hora      Codigo   Operacion   Total Disponible\n", bitacoraRetiro );
	fputs( "-------------------------------------------------------------\n", bitacoraRetiro );
	fputs( "\n", bitacoraRetiro );
	fclose(bitacoraRetiro);

	printf("Introduzca el Codigo del Usuario\n");
	scanf("%d", &IdUsuario);

	for (j; j<i ; j++) 
		{
			if (totalDisponible < 5000)
				{
					printf("AQUI SE MANDA UN MENSAJE AL SERVIDOR PARA RECARGAR\n");	
					totalDisponible = 80000;
				}

			printf("Introduzca que accion desea realizar:\n");
			scanf("%c", &operation[1]);

			if (operation[1] == 'd')
				{
					deposito();
				}
			else if (operation[1] == 'r')
				{
					retiro();
				}
		}
}


// ---------------------------------------- USAR LAS ESTRUCTURAS ----------------------------------------------
void retiro()
{
	int retiro;
	printf("Introduzca el valor del retiro\n");
    scanf("%d", &retiro);

		if 	(retiro <= totalDisponible)
			{
				if ( retiro <= 3000 )
					{
						totalDisponible = totalDisponible - retiro;
		    			printf("Valor del retiro: %d\n", retiro);
		    			printf("Valor del Total Disponible: %d\n",totalDisponible);

		    			time_t tiempo = time(0);
					    struct tm *tlocal = localtime(&tiempo);
					    strftime(fecha, 50, "%d/%m/%y", tlocal);
						strftime(hora, 50, "%H:%M:%S", tlocal);

		    			imprimeTicket(&IdUsuario);
		    			registrarRetiroEnBitacora();

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

void deposito()
{
	int deposito;
	printf("Introduzca el valor del deposito\n");
    scanf("%d", &deposito);
	totalDisponible = totalDisponible + deposito;

	printf("Valor del deposito: %d\n", deposito);
	printf("Valor del Total Disponible: %d\n",totalDisponible);

	time_t tiempo = time(0);
    struct tm *tlocal = localtime(&tiempo);
    strftime(fecha, 50, "%d/%m/%y", tlocal);
	strftime(hora, 50, "%H:%M:%S", tlocal);

	imprimeTicket(&IdUsuario);
	registrarDepositoEnBitacora();
}
// ---------------------------------------- USAR LAS ESTRUCTURAS ----------------------------------------------



void registrarDepositoEnBitacora()
{
	bitacoraDeposito = fopen ("deposito.txt", "a");
	fprintf(bitacoraDeposito,"%s   %s   %d   Deposito        %d\n",fecha, hora, IdUsuario, totalDisponible);
	fclose(bitacoraDeposito);
}

void registrarRetiroEnBitacora()
{

	bitacoraRetiro = fopen ("retiros.txt", "a");
	fprintf(bitacoraRetiro,"%s   %s   %d    Retiro         %d\n",fecha, hora, IdUsuario, totalDisponible);
	fclose(bitacoraRetiro);
}

void imprimeTicket( int *idUsuario){
	printf("\n");
	printf("Se Imprime el Ticket del Usuario:\n");

		printf("	Fecha: %s\n", fecha);
		printf("	Hora: %s\n", hora);
		printf("	Codigo: %d\n", *idUsuario);
		
		if ( operation[1] == 'd') 
			{
				printf("	Operacion: Deposito\n");
			}
		else
			{
				printf("	Operacion: Retiro\n");
			}
	printf("\n");
		
}