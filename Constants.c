/*
 * Constants.c
 *
 *	Contiene los strings de constantes utilizados por el proyecto
 *
 *  Created on: Oct 12, 2016
 *      Author: francisco
 */

#define BASIC_PERMISSIONS 0666
#define MSG_LEN 500
#define MENU_LEN 800
#define ERROR_LEN 160

const char pthreadCreateError[ERROR_LEN] = "pthreadCreateError";
const char pthreadJoinError[ERROR_LEN] = "pthreadJoinError";
const char mkfifoError[ERROR_LEN] = "mkfifo Error";
const char selectError[ERROR_LEN] = "select Error";
const char openDirError[ERROR_LEN] = "openDir Error";
const char openError[ERROR_LEN] = "open Error";
const char getcwdError[ERROR_LEN] = "getcwd() Error";
const char mallocError[ERROR_LEN] = "malloc Error";
const char dirNotExistsError[ERROR_LEN] = "El Directorio deseado no existe";
const char argNumError[ERROR_LEN] = "Numero Incorrecto de Argumentos";
const char argOrdError[ERROR_LEN] = "Orden Incorrecto de Argumentos";
const char invOrdError[ERROR_LEN] = "Argumentos Invalidos";
const char maxDirError[ERROR_LEN] = "La pila de directorios ha llegado a su capacidad maxima, por favor aumente"
		" el valor de MAXDIR al inicio del de usoDiscoMain.c para poder continuar";
const char maxThreadError[ERROR_LEN] = "Ha elegido mas de la capacidad maxima de Threads para el programa, por favor"
		" aumente el valor de MAXTHREADS al inicio del de usoDiscoMain.c para poder continuar";
