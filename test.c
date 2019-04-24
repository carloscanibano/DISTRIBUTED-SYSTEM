#include <mqueue.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "linked_list.h"

int main() {
	Topic_list topics = NULL;
	User_list tenis_users = NULL;
	User_list basket_users = NULL;

	//Insertar algunos usuarios en la lista de usuarios de tenis
	struct user *us1 = (struct user *) malloc(sizeof(struct user));
	us1->ip = 10000;
	us1->port = 50000;

	struct user *us2 = (struct user *) malloc(sizeof(struct user));
	us2->ip = 20000;
	us2->port = 60000;

	struct user *us3 = (struct user *) malloc(sizeof(struct user));
	us3->ip = 30000;
	us3->port = 70000;

	insert_user(&tenis_users, us1);
	insert_user(&tenis_users, us2);
	insert_user(&tenis_users, us3);

	//Insertar algunos usuarios en la lista de usuarios de baloncesto
	struct user *us4 = (struct user *) malloc(sizeof(struct user));
	us4->ip = 12343;
	us4->port = 80000;

	struct user *us5 = (struct user *) malloc(sizeof(struct user));
	us5->ip = 11111;
	us5->port = 90000;

	struct user *us6 = (struct user *) malloc(sizeof(struct user));
	us6->ip = 23432;
	us6->port = 95000;

	insert_user(&basket_users, us4);
	insert_user(&basket_users, us5);
	insert_user(&basket_users, us6);

	struct topic *tp1 = (struct topic *) malloc(sizeof(struct topic));
	strcpy(tp1->name, "Tenis");
	tp1->user_list = tenis_users;

	struct topic *tp2 = (struct topic *) malloc(sizeof(struct topic));
	strcpy(tp2->name, "Basket");
	tp2->user_list = basket_users;

	//Insertando tema en la lista de temas
	insert_topic(&topics, tp1);
	//Insertando tema en la lista de temas
	insert_topic(&topics, tp2);

	//Insertar un usuario a un tema que existe
	insert_user_topic(&topics, "Tenis", 2323, 2323);
	insert_user_topic(&topics, "Basket", 5434, 5434);

	//Insertar un usuario a un tema que no existe
	insert_user_notopic(&topics, "Hockey", 2222, 2222);
	insert_user_notopic(&topics, "Padel", 3333, 3333);

	//Borrar un usuario de un tema que existe
	delete_user_topic(&topics, "Tenis", 10000, 50000);

	//Borrar un usuario de un tema que no existe
	delete_user_topic(&topics, "Armas", 23432, 95000);

	//Mostrar datos actualmente
	show(topics);

	printf("\n\n");
	//Imprimimos el numero de temas
	printf("El numero de temas en la lista es: %d\n", topic_elements(topics));
	//Imprimimos el numero de usuarios
	printf("El numero de usuarios en la lista es: %d\n", user_elements(topics->next_topic->user_list));
	return 0;
}