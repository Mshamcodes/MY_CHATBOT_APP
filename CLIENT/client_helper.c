// ----------------------------------------------
// Project: MY_NEW_CHATBOT_APP
// File: CLEINT/client_helper.c
// Description: Helper functions for the chat client

#include "client_helper.h"


void die(const char *msg) {
    perror(msg);
    exit(1);
}