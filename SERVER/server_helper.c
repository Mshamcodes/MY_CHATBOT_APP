//------------------------------------------------------------------------------
// File: server_helper.c
// Description: Helper functions for the chat server
//------------------------------------------------------------------------------


#include "server.h"
#include "server_helper.h"

struct client_info clients[MAX_CLIENTS];
int server_fd = -1;

// --- Function: init_clients ---
// Initializes the clients array
void init_clients(void) {
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        clients[i].fd = -1;
        clients[i].registered = 0;
        clients[i].name[0] = '\0';
    }
}

// --- Function: trim ---
// Trims leading and trailing whitespace from a string
void trim(char *s) {
    char *start = s;
    while (*start && (*start == ' ' || *start == '\t' || *start == '\r' || *start == '\n'))
        start++;
    char *end = start + strlen(start);
    while (end > start && (end[-1] == ' ' || end[-1] == '\t' || end[-1] == '\r' || end[-1] == '\n'))
        *--end = '\0';
    if (start != s) memmove(s, start, end - start + 1);
}

// --- Function: add_client ---
// Adds a new client to the clients array
// Returns the index of the new client, or -1 if full
int add_client(int fd, struct sockaddr_in *addr) {
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].fd == -1) {
            clients[i].fd = fd;
            clients[i].addr = *addr;
            return i;
        }
    }
    return -1;
}

// --- Function: remove_client ---
// Removes a client from the clients array  
void remove_client(int i) {
    if (i < 0 || i >= MAX_CLIENTS) return;
    if (clients[i].fd != -1) close(clients[i].fd);
    clients[i].fd = -1;
    clients[i].registered = 0;
    clients[i].name[0] = '\0';
}

// --- Function: list_clients ---
// Sends the list of registered clients to the given file descriptor
void list_clients(int fd) {
    dprintf(fd, COL_CYAN "[Server] Connected clients:\n" COL_RESET);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].fd != -1 && clients[i].registered) {
            dprintf(fd, "  %s\n", clients[i].name);
        }
    }
}

// --- Function: find_client_by_name ---
// Returns the index of the client with the given name, or -1 if not found
int find_client_by_name(const char *target) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].fd != -1 &&
            clients[i].registered &&
            strcmp(clients[i].name, target) == 0) {
            return i;
        }
    }
    return -1;
}

// --- Function: broadcast ---
// Sends a message from sender_idx to all other registered clients
void broadcast(int sender_idx, const char *msg) {
    for (int j = 0; j < MAX_CLIENTS; j++) {
        if (clients[j].fd != -1 && clients[j].registered && j != sender_idx) {
            dprintf(clients[j].fd,
                    COL_MAGENTA "[%s] %s\n" COL_RESET,
                    clients[sender_idx].name, msg);
        }
    }
}

// --- Function: cleanup ---
// Cleans up resources on server shutdown
void cleanup(int sig) {
    (void)sig;
    printf(COL_YELLOW "\n[Server] Shutting down...\n" COL_RESET);
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i].fd != -1) close(clients[i].fd);
    if (server_fd != -1) close(server_fd);
    exit(0);
}