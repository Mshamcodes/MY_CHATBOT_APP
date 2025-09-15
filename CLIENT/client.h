// ----------------------------------------------
// Project: MY_NEW_CHATBOT_APP
// File: CLEINT/client.h
// Description: Header file for the chat client


#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>

#define MAX_NAME    128
#define MAX_LINE    512
 
/* ---------- ANSI Color Codes ---------- */
#define COL_RESET   "\x1b[0m"
#define COL_RED     "\x1b[31m"
#define COL_GREEN   "\x1b[32m"
#define COL_YELLOW  "\x1b[33m"
#define COL_BLUE    "\x1b[34m"
#define COL_MAGENTA "\x1b[35m"
#define COL_CYAN    "\x1b[36m"
#define COL_WHITE   "\x1b[37m"

#endif // CLIENT_H