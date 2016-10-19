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
void retiros();
void deposito();
void registrarDepositoEnBitacora();
void registrarRetiroEnBitacora();
void imprimeTicket();


/* GLOBAL VARIABLES */

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
	fputs( "---------------------Bitacora de Retiros --------------------\n", bitacoraRetiro );
	fputs( "\n", bitacoraRetiro );
	fputs( "  Fecha      Hora      Codigo   Operacion   Total Disponible\n", bitacoraRetiro );
	fputs( "-------------------------------------------------------------\n", bitacoraRetiro );
	fputs( "\n", bitacoraRetiro );
	fclose(bitacoraRetiro);

	printf("Introduzca el Codigo del Usuario\n");
	scanf("%d", &IdUsuario);

	while (operation[1] != 'f')
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
					retiros();
				}
		}
}


// ---------------------------------------- USAR LAS ESTRUCTURAS ----------------------------------------------
void retiros()
{
	struct transaction retiro;
	printf("Introduzca el valor del retiro\n");
    scanf("%d", &retiro.amount);

		if 	(retiro.amount <= totalDisponible)
			{
				if ( retiro.amount <= 3000 )
					{
						totalDisponible = totalDisponible - retiro.amount;
		    			printf("Valor del retiro: %d\n", retiro.amount);
		    			printf("Valor del Total Disponible: %d\n",totalDisponible);

		    			time_t tiempo = time(0);
					    struct tm *tlocal = localtime(&tiempo);
					    strftime(retiro.date, 50, "%d/%m/%y", tlocal);
						strftime(retiro.time, 50, "%H:%M:%S", tlocal);

		    			imprimeTicket(&IdUsuario, retiro);
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

void deposito()
{
	struct transaction deposito;
	printf("Introduzca el valor del deposito\n");
    scanf("%d", &deposito.amount);
	totalDisponible = totalDisponible + deposito.amount;

	printf("Valor del deposito: %d\n", deposito.amount);
	printf("Valor del Total Disponible: %d\n",totalDisponible);

	time_t tiempo = time(0);
    struct tm *tlocal = localtime(&tiempo);
    strftime(deposito.date, 50, "%d/%m/%y", tlocal);
	strftime(deposito.time, 50, "%H:%M:%S", tlocal);

	imprimeTicket(&IdUsuario, deposito);
	registrarDepositoEnBitacora(deposito);
}
// ---------------------------------------- USAR LAS ESTRUCTURAS ----------------------------------------------



void registrarDepositoEnBitacora(struct transaction trans)
{
	bitacoraDeposito = fopen ("deposito.txt", "a");
	fprintf(bitacoraDeposito,"%s   %s   %d   Deposito        %d\n", trans.date, trans.time, IdUsuario, totalDisponible);
	fclose(bitacoraDeposito);
}

void registrarRetiroEnBitacora(struct transaction trans)
{
	bitacoraRetiro = fopen ("retiros.txt", "a");
	fprintf(bitacoraRetiro,"%s   %s   %d    Retiro         %d\n",  trans.date, trans.time, IdUsuario, totalDisponible);
	fclose(bitacoraRetiro);
}

void imprimeTicket( int *idUsuario, struct transaction trans)
	{
	printf("\n");
	printf("Se Imprime el Ticket del Usuario:\n");

		printf("	Fecha: %s\n", trans.date);
		printf("	Hora: %s\n", trans.time);
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