#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "linked_list.h"
#include "lines.h"

struct arg_struct{
	char subscriber_ip[128];
	int socket;
};

struct sockaddr_in server_addr, client_addr;

pthread_mutex_t mutex_mensaje;
pthread_mutex_t mutex_operacion;
int mensaje_no_copiado = 1;
pthread_cond_t cond_mensaje;
Topic_list tl = NULL;

//NO OLVIDARSE DE AÑADIR LA FUNCION 
void print_usage() {
	    printf("Usage: broker -p puerto -r direccion rpc\n");
}

void tratar_peticion(struct arg_struct *args){
		//Bloqueamos la copia de newsd para evitar conflictos
		pthread_mutex_lock(&mutex_mensaje);
		int tsd = args->socket;
		char subscriber_ip[128];
		strcpy(subscriber_ip, args->subscriber_ip);
		mensaje_no_copiado = 0;
		pthread_cond_signal(&cond_mensaje);
		pthread_mutex_unlock(&mutex_mensaje);

		int flag = 1;
		//Hay que sacar la ip del newsd cuando el suscriptor se conecta
		char operacion[12];
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
				struct topic *tp = (struct topic*) malloc(sizeof(struct topic));
				strcpy(tp->name, tema);
				tp->user_list = NULL;
				tp->next_topic = NULL;
				pthread_mutex_lock(&mutex_operacion);
				insert_topic(&tl, tp);
				show(tl);
				pthread_mutex_unlock(&mutex_operacion);
				free(tp);
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
				//unsigned char c = 0;
				//writeLine(tsd, &c, sizeof(c));
				char *ptr;
				long port = strtol(puerto, &ptr, sizeof(puerto));
				printf("Direccion ip: %s\n", subscriber_ip);
				printf("Puerto: %ld\n", port);
				pthread_mutex_lock(&mutex_operacion);
				if (insert_user_notopic(&tl, tema, subscriber_ip, port) == -1) {
					insert_user_topic(&tl, tema, subscriber_ip, port);
				}
				show(tl);
				pthread_mutex_unlock(&mutex_operacion);
			} else if (strcmp(operacion, "UNSUBSCRIBE") == 0) {
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
				//unsigned char c = 0;
				//writeLine(tsd, &c, sizeof(c));
				char *ptr;
				long port = strtol(puerto, &ptr, sizeof(puerto));
				pthread_mutex_lock(&mutex_operacion);
				delete_user_topic(&tl, tema, subscriber_ip, port);
				show(tl);
				pthread_mutex_unlock(&mutex_operacion);
			}
		}
		close(tsd);
}

int main(int argc, char *argv[]) {
	int  option = 0;
	char puerto[256]= "";
	char rpc[256] = "";

	while ((option = getopt(argc, argv,"p:r:")) != -1) {
		switch (option) {
		    	case 'p' : 
				strcpy(puerto, optarg);
		    		break;
	    		case 'r' : 
				strcpy(rpc, optarg);
	    			break;
		    	default: 
				print_usage(); 
		    		exit(-1);
		    }
	}
	if ((strcmp(puerto, "") == 0) || (strcmp(rpc, "") == 0)){
		print_usage(); 
		exit(-1);
	}


	printf("Puerto: %s\n", puerto);

	int sd, newsd;
	socklen_t size;

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

		struct arg_struct *args = malloc(sizeof(struct arg_struct));
		args->socket = newsd;
		strcpy(args->subscriber_ip, inet_ntoa(client_addr.sin_addr));
		printf("IP antes de thread: %s\n", args->subscriber_ip);

		printf("Conexion aceptada\n");
		pthread_create(&thid, &t_attr, (void *)tratar_peticion, (void *)args);

	    pthread_mutex_lock(&mutex_mensaje);
	    while (mensaje_no_copiado){
	      pthread_cond_wait(&cond_mensaje, &mutex_mensaje);
	    }
	    mensaje_no_copiado = 1;
	    pthread_mutex_unlock(&mutex_mensaje);
	    free(args);
	/* transferir datos sobre newsd */
	/* procesar la petición utilizando newsd */
	}
	close(sd);
}
	
