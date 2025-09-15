// ----------------------------------------------
// Project: MY_NEW_CHATBOT_APP
// File: SERVER/server_main.c
// Description: Main file for the chat server
//----------------------------------------------

#include "server.h"
#include "server_helper.h"


int main(int argc, char **argv) 
{
    int port = (argc >= 2) ? atoi(argv[1]) : 12345;
    if (port <= 0) 
    {
        port = 12345;
    } 

    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT,  cleanup);
    signal(SIGTERM, cleanup);

    init_clients();

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) 
    { 
        perror("socket"); 
        return 1; 
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in srv;
    memset(&srv, 0, sizeof srv);
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = htonl(INADDR_ANY);
    srv.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&srv, sizeof srv) < 0) 
    {
        perror("bind"); 
        return 1;
    }
    if (listen(server_fd, 8) < 0) 
    {
        perror("listen"); 
        return 1; 
    }

    printf(COL_GREEN
           "||---------------------- SERVER STARTED --------------------||\n"
           "|| Listening on port %d                                  ||\n"
           COL_RESET, port);

    fd_set master, readfds;
    FD_ZERO(&master);
    FD_SET(server_fd, &master);
    int maxfd = server_fd;
    char buf[MAX_BUFFER];

    while (1) {
        readfds = master;
        if (select(maxfd + 1, &readfds, NULL, NULL, NULL) < 0) 
        {
            if (errno == EINTR)
            {
                continue;
            }
            perror("select");
            break;
        }

        /* New connection */
        if (FD_ISSET(server_fd, &readfds)) 
        {
            struct sockaddr_in cli;
            socklen_t len = sizeof cli;
            int newfd = accept(server_fd, (struct sockaddr*)&cli, &len);
            if (newfd >= 0) 
            {
                int idx = add_client(newfd, &cli);
                if (idx < 0) 
                {
                    close(newfd);
                    remove_client(idx);
                }
                else 
                {
                    FD_SET(newfd, &master);
                    if (newfd > maxfd) 
                    {
                        maxfd = newfd;
                    }
                    char ip[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &cli.sin_addr, ip, sizeof ip);
                    printf(COL_CYAN
                           "New connection: fd=%d, ip=%s, port=%d (awaiting name)\n"
                           COL_RESET, newfd, ip, ntohs(cli.sin_port));
                }
            }
        }

        /* Handle client data */
        for (int i = 0; i < MAX_CLIENTS; i++) 
        {
            int fd = clients[i].fd;
            if (fd == -1) 
            {
                continue;
            }
            if (!FD_ISSET(fd, &readfds)) 
            {
                continue;
            }
            ssize_t n = recv(fd, buf, sizeof(buf) - 1, 0);
            if (n <= 0) 
            {
                if (n == 0)
                {
                    printf(COL_RED "Client fd=%d disconnected (EOF)\n" COL_RESET, fd);
                }
                else
                {
                    printf(COL_RED "Client fd=%d disconnected (error)\n" COL_RESET, fd);
                }
                FD_CLR(fd, &master);
                remove_client(i);
                continue;
            }
            buf[n] = '\0';
            trim(buf);
            if (buf[0] == '\0') 
            {
                continue;
            }
            if (!clients[i].registered) 
            {
                strncpy(clients[i].name, buf, MAX_NAME - 1);
                clients[i].name[MAX_NAME - 1] = '\0';
                clients[i].registered = 1;
                printf(COL_YELLOW "Registered: \"%s\" (fd=%d)\n" COL_RESET,
                       clients[i].name, fd);
                continue;
            }

            /* Commands */
            if (strcasecmp(buf, "LIST") == 0) 
            {
                list_clients(fd);
                continue;
            }
            if (strncasecmp(buf, "BROADCAST ", 10) == 0) 
            {
                const char *msg = buf + 10;
                if (*msg)
                {
                    broadcast(i, msg);
                }
                else
                {
                    dprintf(fd, COL_RED "[Server] Usage: BROADCAST <message>\n" COL_RESET);
                }
                continue;
            }
            if (strncasecmp(buf, "DM ", 3) == 0) 
            {
                const char *p = buf + 3;
                while (*p == ' ') 
                {
                    p++;
                }
                if (*p == '"') 
                {
                    p++;
                    const char *endq = strchr(p, '"');
                    if (endq) 
                    {
                        char target[MAX_NAME];
                        size_t len = endq - p;
                        if (len >= MAX_NAME) 
                        {
                            len = MAX_NAME - 1;
                        }
                        strncpy(target, p, len);
                        target[len] = '\0';

                        const char *msg = endq + 1;
                        while (*msg == ' ') 
                        {
                            msg++;
                        }

                        int j = find_client_by_name(target);
                        if (j >= 0 && *msg) 
                        {
                            dprintf(clients[j].fd,
                                    COL_MAGENTA "[%s] %s\n" COL_RESET,
                                    clients[i].name, msg);
                        } 
                        else 
                        {
                            dprintf(fd,
                                    COL_RED "[Server] Client \"%s\" not found or message empty.\n"
                                    COL_RESET, target);
                        }
                        continue;
                    }
                }
                dprintf(fd,
                        COL_RED "[Server] Usage: DM \"full name\" <message>\n" COL_RESET);
                continue;
            }
            dprintf(fd, COL_RED "[Server] Unknown command.\n" COL_RESET);
        }
    }

    cleanup(0);
    return 0;
}

