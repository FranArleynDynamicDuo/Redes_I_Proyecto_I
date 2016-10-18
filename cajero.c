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

/* FUNCTION SIGNATURES */
void retiro();
void deposito();
void registrarDepositoEnBitacora();
void registrarRetiroEnBitacora();
void imprimeTicket();


/* GLOBAL VARIABLES */
int totalDisponible = 80000;
char operation[1] = "i";
int IdUsuario;
char fecha[50];
char hora[50];

int main(int argc, char *argv[]) {

	int i = 10;					// Iterador Multi-
	int j = 0;

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
}


void registrarDepositoEnBitacora()
{
}

void registrarRetiroEnBitacora()
{
}

void imprimeTicket( int *idUsuario){
	printf("\n");
	printf("Se Imprime el Ticket del Usuario:\n");

		printf("	Fecha: %s\n",output);
		printf("	Hora: %s\n",output);
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