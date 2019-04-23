#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include "lines.h"

pthread_mutex_t mutex_mensaje;
int mensaje_no_copiado = 1;
pthread_cond_t cond_mensaje;

void print_usage() {
	    printf("Usage: broker -p puerto \n");
}

void tratar_peticion(int *newsd){
		//Bloqueamos la copia de newsd para evitar conflictos
		pthread_mutex_lock(&mutex_mensaje);
		int tsd = *newsd;
		mensaje_no_copiado = 0;
		pthread_cond_signal(&cond_mensaje);
		pthread_mutex_unlock(&mutex_mensaje);

		int flag = 1;
		//Hay que sacar la ip del newsd cuando el suscriptor se conecta
		char operacion[11];
		char tema[128];
		char texto[1024];
		char puerto[6];
		struct publication;
		int n;
		while (flag) {
			//LEEMOS OPERACION
			n = readLine(tsd, operacion, sizeof(operacion));
			if((n == -1) || (strcmp(operacion, "exit") == 0)) {
				flag = 0;
				break;
			}
			//SI ES OPERACION, ESPERAMOS 2 MENSAJES MAS
			if(strcmp(operacion, "PUBLISH") == 0) {
				//ESCRIBIMOS LA OPERACION
				writeLine(1, operacion, n);
				//LEEMOS EL TEMA
				n = readLine(tsd, tema, sizeof(tema));
				//printf("%s\n", tema);
				if((n == -1) || (strcmp(tema, "exit") == 0)) {
					flag = 0;
					break;
				}
				//ESCRIBIMOS EL TEMA
				writeLine(1, tema, n);
				//LEEMOS EL TEXTO
				n = readLine(tsd, texto, sizeof(texto));
				if((n == -1) || (strcmp(texto, "exit") == 0)) {
					flag = 0;
					break;
				}
				//ESCRIBIMOS EL TEXTO
				writeLine(1, texto, n);
				flag = 0;
				//GUARDAMOS PUBLICACION EN LISTA
			} else if (strcmp(operacion, "SUBSCRIBE") == 0) {
				//ESCRIBIMOS LA OPERACION
				writeLine(1, operacion, n);
				//LEEMOS EL TEMA
				n = readLine(tsd, tema, sizeof(tema));
				//printf("%s\n", tema);
				if((n == -1) || (strcmp(tema, "exit") == 0)) {
					flag = 0;
					break;
				}
				//ESCRIBIMOS EL TEMA
				writeLine(1, tema, n);
				//LEEMOS EL PUERTO
				n = readLine(tsd, puerto, sizeof(puerto));
				//printf("%s\n", tema);
				if((n == -1) || (strcmp(tema, "exit") == 0)) {
					flag = 0;
					break;
				}
				//ESCRIBIMOS EL PUERTO
				writeLine(1, puerto, n);
				//OPERACION CORRECTA
				//PROBLEMA, NO SE COMO MANDAR EL DATO PARA QUE SE LEA BIEN EN JAVA
				char c = 0;
				writeLine(tsd, &c, sizeof(c));
			}
		}
		close(tsd);
}

int main(int argc, char *argv[]) {
	int  option = 0;
	char puerto[256]= "";

	while ((option = getopt(argc, argv,"p:")) != -1) {
		switch (option) {
		    	case 'p' : 
				strcpy(puerto, optarg);
		    		break;
		    	default: 
				print_usage(); 
		    		exit(-1);
		    }
	}
	if (strcmp(puerto,"")==0){
		print_usage(); 
		exit(-1);
	}


	printf("Puerto: %s\n", puerto);

	int sd, newsd;
	socklen_t size;
	struct sockaddr_in server_addr, client_addr;

	if ((sd=socket(AF_INET, SOCK_STREAM, 0))==-1){
		printf("Error en la creación del socket");
		return(-1);
	}

	//Correccion de errores en ejecuciones simultaneas
	int val = 1;
	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, 
				(void*) &val, sizeof(val));

	bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(puerto));
	server_addr.sin_addr.s_addr = INADDR_ANY;
	/* bind */
	if (bind(sd,(struct sockaddr *)&server_addr,
		sizeof(server_addr)) <0) {
		printf("Error en el bind\n");
		return(-1);
	}
	listen(sd,SOMAXCONN);

	pthread_attr_t t_attr;
 	pthread_t thid;
 	pthread_mutex_init(&mutex_mensaje, NULL);
 	pthread_cond_init(&cond_mensaje, NULL);
 	pthread_attr_init(&t_attr);
 	pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);

	for (;;) {
		newsd=accept (sd, (struct sockaddr *) &client_addr, &size);

		if (newsd < 0) {
			printf("Error en el accept\n");
			return(-1);
		}
		printf("Conexion aceptada\n");
		pthread_create(&thid, &t_attr, (void *)tratar_peticion, &newsd);

	    pthread_mutex_lock(&mutex_mensaje);
	    while (mensaje_no_copiado){
	      pthread_cond_wait(&cond_mensaje, &mutex_mensaje);
	    }
	    mensaje_no_copiado = 1;
	    pthread_mutex_unlock(&mutex_mensaje);

	/* transferir datos sobre newsd */
	/* procesar la petición utilizando newsd */
	}
	close(sd);
}
	
