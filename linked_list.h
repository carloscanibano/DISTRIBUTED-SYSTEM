#ifndef _linked_list_h
#define _linked_list_h

struct topic{
	char name[255];
	struct user *user_list;
	struct topic *next_topic;
};

//NO OLVIDAR IDENTIFICAR EL CLIENTE POR IP Y PUERTO
struct user{
	char ip[128];
	long port;
	struct user *next_user;
};

typedef struct topic *Topic_list;
typedef struct user *User_list;

//Insert topic
int insert_topic(Topic_list *tl, struct topic *t);
//Insert user
int insert_user(User_list *ul, struct user *u);
//Subscribe user to existing topic
int insert_user_topic(Topic_list *tl, char *topic_name, char *ip, long port);
//Subscribe user to non existing topic
int insert_user_notopic(Topic_list *tl, char *topic_name, char *ip, long port);
//Unsubscribe user of existing topic/non existing topic
int delete_user_topic(Topic_list *tl, char *topic_name, char *ip, long port);

//Verify if a topic exists
int verify_topic(Topic_list tl, char *topic_name);
//Verify if a user exists
int verify_user(User_list ul, char *ip, long port);
//Show every topic and user inside the linked list
void show(Topic_list tl);
//Erase the entire linked list topics
int erase_topics(Topic_list *tl);
//Erase the entire linked list users
int erase_users(User_list *ul);
//Count topics
int topic_elements(Topic_list tl);
//Count users
int user_elements(User_list ul);
//Look for a specific topic
struct topic* search_topic(Topic_list tl, char *topic_name);
//Look for a specific user topics
char * search_user_topics(Topic_list tl, char *ip, short port);
/*
//Modify a existing value
int modify(Triplet_list *l, char *key, char *value1, float value2);
//Delete single tuple
int delete(Triplet_list *l, char *key);
*/
#endif

