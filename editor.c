#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include "lines.h"

void print_usage() {
	    printf("Usage: editor -h host -p puerto -t \"tema\" -m \"texto\"\n");
}

int main(int argc, char *argv[]) {
	int  option = 0;
	char host[256]= "";
	char puerto[256]= "";
	char tema[256]= "";
	char texto[1024]= "";
	char operacion[11] = "PUBLISH";

	while ((option = getopt(argc, argv,"h:p:t:m:")) != -1) {
		switch (option) {
		        case 'h' : 
				strcpy(host, optarg);
		    		break;
		    	case 'p' : 
				strcpy(puerto, optarg);
		    		break;
		    	case 't' : 
				strcpy(tema, optarg);
		    		break;
		    	case 'm' : 
				strcpy(texto, optarg);
		    		break;
		    	default: 
				print_usage(); 
		    		exit(-1);
		    }
	}
	if (strcmp(host,"")==0 || strcmp(puerto,"")==0 ||
		strcmp(tema,"")==0 || strcmp(texto,"")==0){
		print_usage(); 
		exit(-1);
	}


	printf("Host: %s\n", host);
	printf("Puerto: %s\n", puerto);
	printf("Tema: %s\n", tema);
	printf("texto: %s\n", texto);

	int sd;      
	short port;
	struct sockaddr_in server_addr;
	struct hostent *hp;

	port = (short) atoi(puerto);
	sd = socket(AF_INET, SOCK_STREAM, 0);        
	bzero((char*)&server_addr, sizeof(server_addr));
	hp = gethostbyname(host);
	memcpy(&(server_addr.sin_addr), hp->h_addr, hp->h_length);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	// se establece la conexión
	if(connect(sd, (struct sockaddr*) &server_addr, 
								sizeof(server_addr)) < 0) {
		printf("Error en la conexión\n");
		return(0);
	}
	/* se usa sd para enviar o recibir datos del servidor */

	//int n;

	for (;;) {
		//char buffer[256];
		//n = readLine(0, buffer, sizeof(buffer));
		enviar(sd, operacion, strlen(operacion) + 1);
		enviar(sd, tema, strlen(tema) + 1);
		enviar(sd, texto, strlen(texto) + 1);
	}

	close(sd);


	return 0;
}
	
