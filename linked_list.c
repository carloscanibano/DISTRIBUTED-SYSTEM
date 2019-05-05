#include "linked_list.h"
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

int insert_topic(Topic_list *tl, struct topic *t){
	Topic_list p1, p2;
	p1 = *tl;
	int code = -1;

	if (p1 == NULL) {
		p1 = (struct topic *) malloc(sizeof(struct topic));
		strcpy(p1->name, t->name);
		p1->user_list = t->user_list;
		p1->next_topic = NULL;
		*tl = p1;
		code = 0;
	} else {
		if (verify_topic(*tl, t->name) == -1){
			while (p1->next_topic != NULL) {
				p1 = p1->next_topic;
			}
			p2 = (struct topic *) malloc(sizeof(struct topic));
			strcpy(p2->name, t->name);
			p2->user_list = t->user_list;
			p1->next_topic = p2;
			code = 0;
		}
	}
	return code;
}

int insert_user(User_list *ul, struct user *u){
	User_list p1, p2;
	p1 = *ul;
	int code = -1;

	if (p1 == NULL) {
		p1 = (struct user *) malloc(sizeof(struct user));
		strcpy(p1->ip, u->ip);
		p1->port = u->port;
		p1->next_user = NULL;
		*ul = p1;
		code = 0;
	} else {
		if (verify_user(*ul, u->ip, u->port) == -1){
			while (p1->next_user != NULL) {
				p1 = p1->next_user;
			}
			p2 = (struct user *) malloc(sizeof(struct user));
			strcpy(p2->ip, u->ip);
			p2->port = u->port;
			p1->next_user = p2;
			code = 0;
		}
	}
	return code;
}

int insert_user_topic(Topic_list *tl, char *topic_name, char *ip, long port){
	int code = -1;
	if (verify_topic(*tl, topic_name) != -1) {
		struct topic* tp = search_topic(*tl, topic_name);
		struct user us;
		strcpy(us.ip, ip);
		us.port = port;
		us.next_user = NULL;
		code = insert_user(&tp->user_list, &us);
	}
	return code;
}

int insert_user_notopic(Topic_list *tl, char *topic_name, char *ip, long port){
	int code = -1;
	if (verify_topic(*tl, topic_name) == -1) {
		struct topic *tp = (struct topic *) malloc(sizeof(struct topic));
		struct user *us = (struct user *) malloc(sizeof(struct user));
		strcpy(us->ip, ip);
		us->port = port;
		us->next_user = NULL;
		strcpy(tp->name, topic_name);
		insert_user(&tp->user_list, us);
		tp->next_topic = NULL;
		insert_topic(tl, tp);
		code = 0;
	}
	return code;
}

int delete_user_topic(Topic_list *tl, char *topic_name, char *ip, long port) {
	User_list aux = NULL;
	int code = -1;
	if (verify_topic(*tl, topic_name) == -1) {
		return code;
	}
	struct topic* tp = search_topic(*tl, topic_name);
	if (verify_user(tp->user_list, ip, port) == -1) {
		return code;
	} else {
			User_list cursor = (*tl)->user_list;
			//printf("CURSOR IP: %s\n", cursor->ip);
			if ((strcmp(cursor->ip, ip) == 0) && (cursor->port == port)) {
				if (cursor->next_user == NULL){
					free(cursor);
					//*tl = NULL;
					tp->user_list = NULL;
					return 0;
				} else {
					tp->user_list = cursor->next_user;
					free(cursor);
					return 0;
				}
			} else {
				while (cursor != NULL) {
				//printf("CURSOR IP ELSE: %s\n", cursor->ip);
				if ((strcmp(cursor->next_user->ip, ip) == 0) && (cursor->next_user->port == port)) {
					aux = cursor->next_user;
					cursor->next_user = cursor->next_user->next_user;
					free(aux);
					return 0;
				}
				cursor = cursor->next_user;
			}
		}
	return code;
	}
}

int verify_topic(Topic_list tl, char *topic_name){
	int code = -1;
	Topic_list cursor = tl;
	while (cursor != NULL) {
		if (strcmp(cursor->name, topic_name) == 0) {
			code = 0;
		}
		cursor = cursor->next_topic;
	}
		return code;
}

int verify_user(User_list ul, char *ip, long port){
	int code = -1;
	User_list cursor = ul;
	while (cursor != NULL) {
		if ((strcmp(cursor->ip, ip) == 0) && (cursor->port == port)) {
			code = 0;
		}
		cursor = cursor->next_user;
	}
		return code;
}

void show(Topic_list tl) {
	struct user* ul;
	while (tl != NULL) {
		printf("Name: %s \n", tl->name);
		ul = tl->user_list;
		while (ul != NULL) {
			printf("IP: %s, puerto: %ld \n", ul->ip, ul->port);
			ul = ul->next_user;
		}
		tl = tl->next_topic;
		//if(tl == NULL)printf("NULL\n");
	}
}

int erase_topics(Topic_list *tl) {
	Topic_list aux, p;
	int code = 0;
	p = *tl;
	if (p != NULL) {
		while (p != NULL) {
			aux = p->next_topic;
			free(p);
			p = aux;
		}
		*tl = NULL;
	}
	return code;
}

int erase_users(User_list *ul) {
	User_list aux, p;
	int code = 0;
	p = *ul;
	if (p != NULL) {
		while (p != NULL) {
			aux = p->next_user;
			free(p);
			p = aux;
		}
		*ul = NULL;
	}
	return code;
}

int topic_elements(Topic_list tl) {
	Topic_list cursor = tl;
	int c = 0;
	while (cursor != NULL) {
		c++;
		cursor = cursor->next_topic;
	}
	return c;
}

int user_elements(User_list ul) {
	User_list cursor = ul;
	int c = 0;
	while (cursor != NULL) {
		c++;
		cursor = cursor->next_user;
	}
	return c;
}

struct topic* search_topic(Topic_list tl, char *name) {
	struct topic *not_found = (struct topic *) malloc(sizeof(struct topic));
	Topic_list cursor = tl;
	while (cursor != NULL) {
		if (strcmp(cursor->name, name) == 0) {
			free(not_found);
			return cursor;
		}
		cursor = cursor->next_topic;
	}
	strcpy(not_found->name, "NOT_FOUND");
	return not_found;
}

int quit(Topic_list *tl,  char *ip, long port) {
	Topic_list cursor = *tl;
	int code = -1;

	while (cursor != NULL) {
		if (verify_user(cursor->user_list, ip, port) != -1) {
			if(delete_user_topic(&cursor, cursor->name, ip, port) == -1) {
				return -1;
			} else {
				code = 1;
			}
		}
		cursor = cursor->next_topic;
	}

	return code;
}
/*
int modify(Publication_list *l, char *key, char *value1, float value2) {
	int code = -1;
	Publication_list cursor = *l;
	if (verify(*l, key) == -1) {
		return code;
	} else {
		while (cursor != NULL) {
			if (strcmp(cursor->key, key) == 0) {
				strcpy(cursor->first_value, value1);
				cursor->second_value = value2;
				return 0;
			}
			cursor = cursor->link;
		}
	}
	return code;
}
int verify(Publication_list l, char *key){
	int code = -1;
	Publication_list cursor = l;
	while (cursor != NULL) {
		if (strcmp(cursor->key, key) == 0) {
			code = 0;
		}
		cursor = cursor->link;
	}
		return code;
}
*/