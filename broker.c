#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
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
#include <netdb.h>
#include "linked_list.h"
#include "lines.h"
#include "topic_server.h"

//Estructura necesaria para pasar al thread sus parametros unicos
struct arg_struct{
	char subscriber_ip[128];
	int socket;
};

//Establecer estructura de consulta de RPCs
CLIENT *clnt;

struct sockaddr_in server_addr, client_addr;

pthread_mutex_t mutex_mensaje;
pthread_mutex_t mutex_operacion;
int mensaje_no_copiado = 1;
pthread_cond_t cond_mensaje;
Topic_list tl = NULL;

void print_usage() {
	    printf("Usage: broker -p puerto -r direccion rpc\n");
}

void tratar_peticion(struct arg_struct *args){
		//Sirve para evaluar resultado de operacion RPC
		enum clnt_stat retval;
		//Bloqueamos la copia de parametros para evitar condiciones de carrera
		pthread_mutex_lock(&mutex_mensaje);
		int tsd = args->socket;
		char subscriber_ip[128];
		strcpy(subscriber_ip, args->subscriber_ip);
		mensaje_no_copiado = 0;
		pthread_cond_signal(&cond_mensaje);
		pthread_mutex_unlock(&mutex_mensaje);
		//Bandera para realizar el bucle de operaciones
		int flag = 1;
		char operacion[12];
		char tema[130];
		char texto[1030];
		char puerto[10];
		//Numero de bytes escritos/leidos
		int n;
		while (flag) {
			//Leemos la operacion recibida
			n = readLine(tsd, operacion, sizeof(operacion));
			if((n == -1) || (strcmp(operacion, "exit") == 0)) {
				flag = 0;
				break;
			}
			//Gestionamos la publicacion de un mensaje
			if(strcmp(operacion, "PUBLISH") == 0) {
				//Escritura de la operacion por pantalla
				//writeLine(1, operacion, n);
				//Leemos el tema de la publicacion
				n = readLine(tsd, tema, sizeof(tema));
				if((n == -1) || (strcmp(tema, "exit") == 0)) {
					flag = 0;
					break;
				}
				//Escribimos el tema recibido
				//writeLine(1, tema, n);
				//Leemos el texto de la publicacion
				n = readLine(tsd, texto, sizeof(texto));
				if((n == -1) || (strcmp(texto, "exit") == 0)) {
					flag = 0;
					break;
				}
				//Escribimos el texto de la publicacion
				//writeLine(1, texto, n);
				//Debemos interrumpir el bucle de operaciones, la publicacion es unica por llamada
				flag = 0;
				//Guardamos el nuevo tema de publicacion en la lista enlazada
				struct topic *tp = (struct topic*) malloc(sizeof(struct topic));
				strcpy(tp->name, tema);
				tp->user_list = NULL;
				tp->next_topic = NULL;

				//Guardamos el texto asociado a la publicacion en el servidor RPC
				int res;
				retval = store_topic_text_1(tema, texto, &res, clnt);
				if (retval !=  RPC_SUCCESS) {
					clnt_perror(clnt, "call failed:");
				}

				if (res == 0) {
					printf("Texto insertado correctamente\n");
				} else {
					printf("Problema al insertar el texto\n");
				}
				//Bloqueamos operacion con lista para evitar condiciones de carrera
				pthread_mutex_lock(&mutex_operacion);
				insert_topic(&tl, tp);
				struct topic *sub = search_topic(tl, tema);
				pthread_mutex_unlock(&mutex_operacion);
				//Liberamos la memoria reservada para el tema
				free(tp);
				User_list subs = sub->user_list;
				//Iniciamos mecanismo de envio a multiples suscriptores al mismo tema
				if(strcmp(sub->name, "NOT_FOUND") != 0){
					//Anadimos una barra para luego separar el mensaje en el suscriptor
					strcat(tema, "/");
					//Mientras quedan suscriptores asociados al tema...
					while(subs != NULL){
						//Iniciamos la creacion del socket
						struct sockaddr_in sub_addr;
						int sock;
						if ((sock=socket(AF_INET, SOCK_STREAM, 0))==-1){
							printf("Error en la creación del socket");
						}
						struct hostent *hp;
						bzero((char*)&sub_addr, sizeof(sub_addr));
						hp = gethostbyname(subs->ip);
						memcpy(&(sub_addr.sin_addr), hp->h_addr, hp->h_length);
						sub_addr.sin_family = AF_INET;
						sub_addr.sin_port = htons((short)subs->port);
						//Establecemos la conexion con el suscriptor
						if(connect(sock, (struct sockaddr*) &sub_addr,
													sizeof(sub_addr)) < 0) {
							printf("Error in the connection to subscriber\n");
							//Si no es posible la conexion, se le quita la suscripcion
							delete_user_topic(&tl, tema, subs->ip, subs->port);
						}
						//Envio del tema al suscriptor
						if(enviar(sock, tema, strlen(tema) + 1) == -1){
							printf("Error en el tema\n");
						}else{
							//printf("Enviado tema\n");
						}
						//Envio del texto al suscriptor
						if(enviar(sock, texto, strlen(texto) + 1) == -1){
							printf("Error en el texto\n");
						}else{
							//printf("Enviado texto\n");
						}
						//Buscamos el siguiente usuario potencial
						subs = subs->next_user;
						//Cerramos el socket
						close(sock);
					}
				}
				//printf("\n------------------------\n");
			//Gestionamos la suscripcion al mensaje
			} else if (strcmp(operacion, "SUBSCRIBE") == 0) {
				//Escribimos la operacion realizada
				//writeLine(1, operacion, n);
				//Leemos el tema recibido
				n = readLine(tsd, tema, sizeof(tema));
				if((n == -1) || (strcmp(tema, "exit") == 0)) {
					flag = 0;
					break;
				}
				//Escribimos el tema recibido
				//writeLine(1, tema, n);
				//Recibimos el puerto de escucha del suscriptor
				n = readLine(tsd, puerto, sizeof(puerto));
				if((n == -1) || (strcmp(tema, "exit") == 0)) {
					flag = 0;
					break;
				}
				//Escribimos el puerto recibido
				//writeLine(1, puerto, n);
				//Transformamos el puerto en tipo long
				long port = strtol(puerto, NULL, 10);
				//Bloqueamos operaciones con lista para evitar condiciones de carrera
				pthread_mutex_lock(&mutex_operacion);
				char *r1 = "0";
				char *r2 = "1";
				//Si el topic no existe, lo creamos tambien aparte de insertar el usuario
				int resp = insert_user_notopic(&tl, tema, subscriber_ip, port);
				if (resp == -1) {
					//Si el topic existe, solo insertamos el usuario en su lista de usuarios
					resp = insert_user_topic(&tl, tema, subscriber_ip, port);
				}
				if(resp == 0){
					if(enviar(tsd, r1, strlen(r1) + 1) == 0){
						//printf("ENVIADO 0\n");
					}else{
						printf("NO ENVIADO 0\n");
					}
				}else{
					if(enviar(tsd, r2, strlen(r2) + 1) == 0){
						//printf("ENVIADO 1\n");
					}else{
						printf("NO ENVIADO 1\n");
					}
				}
				//Mostramos el estado de la lista despues de las operaciones
				//show(tl);
				pthread_mutex_unlock(&mutex_operacion);
				//Consultamos el ultimo texto disponible para ese tema mediante RPC
				char *ultimo_texto = malloc(1025);
				retval = retrieve_topic_text_1(tema, &ultimo_texto, clnt);
				if (retval !=  RPC_SUCCESS) {
					clnt_perror(clnt, "call failed:");
				}
				//Si existia con anterioridad el topic, entonces mandamos su ultimo texto no vacio
				if (strcmp(ultimo_texto, "") != 0) {
					//Creamos el socket correspondiente
					struct sockaddr_in sub_addr;
					int sock;
					if ((sock=socket(AF_INET, SOCK_STREAM, 0))==-1){
						printf("Error en la creación del socket");
					}
					//printf("Creo bien el socket\n");
					//printf("Usuario: %s\n", subscriber_ip);
					//printf("PORT USUARIO: %ld\n", port);
					struct hostent *hp;
					bzero((char*)&sub_addr, sizeof(sub_addr));
					hp = gethostbyname(subscriber_ip);
					memcpy(&(sub_addr.sin_addr), hp->h_addr, hp->h_length);
					//sub_addr.sin_addr.s_addr = *(long *) hp->h_addr;
					sub_addr.sin_family = AF_INET;
					sub_addr.sin_port = htons((short) port);

					if(connect(sock, (struct sockaddr*) &sub_addr,
												sizeof(sub_addr)) < 0) {
						printf("Error en la conexión\n");
					}
					//Concatenamos una barra para separar el texto del tema en el suscriptor
					strcat(tema, "/");
					//Enviamos tema y texto
					if(enviar(sock, tema, strlen(tema) + 1) == -1){
						printf("Error en el tema\n");
					}else{
						//printf("Enviado tema\n");
					}
					if(enviar(sock, ultimo_texto, strlen(ultimo_texto) + 1) == -1){
						printf("Error en el texto\n");
					}else{
						//printf("Enviado texto\n");
					}
					close(sock);
				}
				free(ultimo_texto);
				//printf("\n------------------------\n");
			//Gestionamos dejar de estar suscrito a un tema
			} else if (strcmp(operacion, "UNSUBSCRIBE") == 0) {
				//Escribimos la operacion recibida
				writeLine(1, operacion, n);
				//Leemos el tema que nos envian
				n = readLine(tsd, tema, sizeof(tema));
				if((n == -1) || (strcmp(tema, "exit") == 0)) {
					flag = 0;
					break;
				}
				//Escribimos el tema recibido
				writeLine(1, tema, n);
				//Leemos el puerto de escucha del suscriptor
				n = readLine(tsd, puerto, sizeof(puerto));
				if((n == -1) || (strcmp(tema, "exit") == 0)) {
					flag = 0;
					break;
				}
				//Escribimos el puerto recibido
				writeLine(1, puerto, n);
				//Convertimos el puerto a tipo long
				long port = strtol(puerto, NULL, 10);
				//Bloqueamos las operaciones de lista para evitar condiciones de carrera
				pthread_mutex_lock(&mutex_operacion);
				char *r1 = "0";
				char *r2 = "1";
				//Borramos un usuario concreto del topic que nos envian
				int resp = delete_user_topic(&tl, tema, subscriber_ip, port);
				if(resp == 0){
					enviar(tsd, r1, strlen(r1) + 1);
				}else{
					enviar(tsd, r2, strlen(r2) + 1);
				}
				//show(tl);
				pthread_mutex_unlock(&mutex_operacion);
				//printf("\n------------------------\n");
			//Mostramos la operacion de quitar la suscripcion de un usuario de todos los temas
			} else if (strcmp(operacion, "QUIT") == 0){
				//Escribimos la operacion recibida
				//writeLine(1, operacion, n);
				//Leemos el puerto de escucha
				n = readLine(tsd, puerto, sizeof(puerto));
				//Escribimos el puerto recibido
				//writeLine(1, puerto, n);
				//Transformamos el puerto a long
				long port = strtol(puerto, NULL, 10);
				pthread_mutex_lock(&mutex_operacion);
				//Iniciamos la operacion quit
				if(quit(&tl, subscriber_ip, port) == -1){
					printf("[ERROR] EN QUIT\n");
				}
				//show(tl);
				pthread_mutex_unlock(&mutex_operacion);
				//No queremos hacer mas operaciones con el suscriptor
				flag = 0;
				//printf("\n------------------------\n");
			}
		}
		close(tsd);
}

int main(int argc, char *argv[]) {
	//Sirve para evaluar la respuesta RCP
	enum clnt_stat retval;
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
	//Localizar el servidor RPC
	clnt = clnt_create(rpc, topic_server, topic_server_v1, "tcp");
	if (clnt == NULL) {
		clnt_pcreateerror(rpc);
		return -1;
	}
	//Iniciamos el servicio de BBDD mediante RPC
	int res;
	retval = init_1(&res, clnt);
	if (retval !=  RPC_SUCCESS) {
		clnt_perror(clnt, "call failed:");
	}

	/*
	if (res == 0) {
		printf("Servicio de BBDD inicializado correctamente\n");
	} else {
		printf("Problema al iniciar el servicio de BBDD\n");
	}
	*/

	int sd, newsd;
	socklen_t size = sizeof client_addr;

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

		//printf("IP cliente: %s\n", inet_ntoa(client_addr.sin_addr));
		struct arg_struct *args = malloc(sizeof(struct arg_struct));
		args->socket = newsd;
		strcpy(args->subscriber_ip, inet_ntoa(client_addr.sin_addr));

		pthread_create(&thid, &t_attr, (void *)tratar_peticion, (void *)args);

	    pthread_mutex_lock(&mutex_mensaje);
	    while (mensaje_no_copiado){
	      pthread_cond_wait(&cond_mensaje, &mutex_mensaje);
	    }
	    mensaje_no_copiado = 1;
	    pthread_mutex_unlock(&mutex_mensaje);
	    free(args);
	}
	close(sd);
	clnt_destroy(clnt);
}