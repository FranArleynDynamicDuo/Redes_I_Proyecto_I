# Redes_I_Proyecto_I
Proyecto I De Redes I

------------------------------------------------------------------------------

				PROYECTO DE SOCKETS
		       	      REDES DEL COMPUTADOR 1

Francisco Sucre 	# 10-10717
Arleyn Goncalves	# 10-10290
------------------------------------------------------------------------------

1. Archivos que contiene el .targ

	- Makefile
		Archivo que nos permite compilar todos los archivos requeridos.
		Al ingresar el comando make en el terminal, se obtienen todos 
		los archivos .o necesarios para correr el programa.

	- Cliente.c
		Contiene todas la funciones respectivas del cliente como cajero 
		automático, verifica los comandos ingresados y contiene el código del 
		socket que corresponde al cliente.
		
	- Server.c
		Contiene todas la funciones respectivas del sistema central que 
		trabaja la red, verifica los comandos ingresados y contiene el código del 
		socket que corresponde al servidor.

		Además tiene las siguientes funciones:
		
			- inicializarBitacoraDeposito: para crear e iniciar el archivo .txt
			  donde se registraran los depósitos del sistema.

			- inicializarBitacoraRetiro: para crear e iniciar el archivo .txt
			  donde se registraran los retiros del sistema.
 
			- sigkillHandler: manejador de señales usado en dado caso que el 
			  servidor se cierre inesperadamente.

			- retiro: se manejan todas las operaciones respectivas al retiro
			  de dinero en el cajero automático.

			- deposito: se manejan todas las operaciones respectivas al deposito
			  de dinero en el cajero automático.

			- registrarDepositoEnBitacora: registra en la bitácora
			  ( un archivo .txt inicializado anteriormente) todos los retiros 
			  realizados en los cajeros automáticos.

			- registrarRetiroEnBitacora: registra en la bitácora
			  ( un archivo .txt inicializado anteriormente) todos los retiros 
			  realizados en los cajeros automáticos.

			- imprimeTicket: muestra en el terminal los datos relacionados a la 
			  operación realizada (deposito/retiro) por el usuario.

			- efectuarOperacion: funcion auxiliar que retorna -1 si tiene un error
			  y 0 si se ejecuto correctamente.

	- Constants.c
		Contiene los strings de constantes utilizados por el proyecto

	- Constants.h
		Contiene las definiciones de los strings de constantes utilizados 
		por el proyecto.

	- ErrorHandling.c
		Archivo que contiene funciones para imprimir mensajes de error
		y funciones que sirvan como manejadores de excepciones.

	- ErrorHandling.h
		Archivo que contiene funciones para imprimir mensajes de error
		y funciones que sirvan como manejadores de excepciones.


2. ¿Que hace el Programa?

	El programa es una simulación de la conexión entre un cajero automático que es
	un computador personal (PC) y un sistema central que trabaja en la red.

	El cajero tiene funciones completas, es decir, se puede depositar y retirar dinero.

	El objetivo es diseñar un sistema informático basandonos en la arquitectura
	Cliente/Servidor que permita automatizar y controlar la red de comunicación 
	entre el cajero automático y el sistema central. 

3. ¿Como se Ejecuta el Programa?

	1. Se abre un terminal y se corre el comando "make", para
	compilar el programa y obtener todos los archivo .o correspondientes.

	2. Se abre un segundo terminal y se realiza la invocación de los comandos para
	el servidor.

		./bsb_svr -l <puerto_bsb_svr> -i <bitácora_depósito> -o <bitácora_retiro>

		Donde:

		<puerto_bsb_svr>: es el número del puerto local en que el computador central
		ofrecera el servicio.

		<bitácora_depósitos>: es le nombre y la dirección relativa y absoluta de un archivo
		de texto que almacena las operaciones de depósito del cajero.

		<bitácora_retiro>: es le nombre y la dirección relativa y absoluta de un archivo
		de texto que almacena las operaciones de retiro del cajero.

		Por Ejemplo:

		./bsb_svr -l 20290 -i deposito.txt -o retiro.txt

	3. Se abre un tercer terminal y se realiza la invocación de comandos pero esta 
	vez para el cliente.

		./bsb_cli -d <nombre_módulo_atención> -p <puerto_bsb_svr> -c <op> (r/d) -i <codigo_usuario>

		Donde:

		<nombre_módulo_atención>: Es el nombre de dominio o de la direccion IP (versión 4) 
		del equipo en donde se deberá ejecutar el módulo de atención centralizada.

		<puerto_bsb_svr>: Es el número de puerto remoto en que el módulo de servicio
		atenderá la comunicación solicitada.

		<op>: Indica si el usuario va a depositar o retirar dinero del cajero, puede tener
		dos valores d ó r.

		<codigo_usuario>: Es un número serial que identifica un usuario de forma única.

		Por Ejemplo: 
	
		./bsb_cli -d localhost -p 20290 -c r -i 1010290
				

4. ¿Condiciones particulares del programa?







