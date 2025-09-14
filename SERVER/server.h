// ----------------------------------------------
 // Project: MY_NEW_CHATBOT_APP
 // File: SERVER/server.h
 // Description: Header file for the chat server
 //----------------------------------------------

#ifndef SERVER_H
#define SERVER_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <signal.h>
#include <time.h>


/* ---------- ANSI Color Codes ---------- */
#define COL_RESET   "\x1b[0m"
#define COL_RED     "\x1b[31m"
#define COL_GREEN   "\x1b[32m"
#define COL_YELLOW  "\x1b[33m"
#define COL_BLUE    "\x1b[34m"
#define COL_MAGENTA "\x1b[35m"
#define COL_CYAN    "\x1b[36m"
#define COL_WHITE   "\x1b[37m"

#define MAX_CLIENTS 32
#define MAX_NAME    128
#define MAX_BUFFER  512

struct client_info {
    int fd;
    char name[MAX_NAME];
    struct sockaddr_in addr;
    int registered;
};

extern struct client_info clients[MAX_CLIENTS];
extern int server_fd;

#endif // SERVER_H

