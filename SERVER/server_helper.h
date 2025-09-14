// ----------------------------------------------
// Project: MY_NEW_CHATBOT_APP
// File: SERVER/server_helper.h
// Description: Header for server helper functions


#ifndef SERVER_HELPER_H
#define SERVER_HELPER_H

void init_clients(void);
void trim(char *s);
int add_client(int fd, struct sockaddr_in *addr);
void remove_client(int i);
void list_clients(int fd);
int find_client_by_name(const char *target);
void broadcast(int sender_idx, const char *msg);
void cleanup(int sig);

#endif // SERVER_HELPER_H