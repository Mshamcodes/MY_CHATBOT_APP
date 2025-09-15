// ----------------------------------------------
// Project: MY_NEW_CHATBOT_APP
// File: CLEINT/client.c
// Description: Main file for the chat client


#include "client.h"
#include "client_helper.h"


int main(int argc, char *argv[]) {
    if (argc < 3) 
    {
        fprintf(stderr,"Usage: %s <server_ip> <port>\n", argv[0]);
        return 1;
    }
    const char *server_ip = argv[1];
    int port = atoi(argv[2]);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) 
    {
        die("socket");
    }

    struct sockaddr_in srv;
    memset(&srv,0,sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_port   = htons(port);
    if (inet_pton(AF_INET, server_ip, &srv.sin_addr) <= 0) 
    {
        die("inet_pton");
    }

    if (connect(sock, (struct sockaddr*)&srv, sizeof(srv)) < 0) 
    {
        die("connect");
    }
    printf(COL_GREEN "Connected to server %s:%d\n" COL_RESET, server_ip, port);

    // register name
    char name[MAX_NAME];
    printf(COL_YELLOW "Enter your name: " COL_RESET);
    fflush(stdout);
    if (!fgets(name, sizeof(name), stdin)) 
    {
        return 0;
    }
    name[strcspn(name, "\n")] = 0;
    /* trim leading and trailing whitespace so registered names match reliably */
    char *s = name;
    while (*s && isspace((unsigned char)*s)) 
    {
        s++;
    }
    if (s != name) 
    {
        memmove(name, s, strlen(s)+1);
    }
    /* trim trailing spaces */
    size_t ln = strlen(name);
    while (ln > 0 && isspace((unsigned char)name[ln-1])) 
    {
        name[--ln] = '\0';
    }
    send(sock, name, strlen(name), 0);
    printf(COL_BLUE "Name registered as \"%s\".\n" COL_RESET, name);

    printf(COL_CYAN "Type commands:\n" COL_RESET);
    printf(COL_CYAN " 1. LIST  - list clients\n" COL_RESET);
    printf(COL_CYAN " 2. quit  - exit\n" COL_RESET);
    printf("\n");

    fd_set rfds;
    char line[MAX_LINE];

    for (;;) {
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        FD_SET(sock, &rfds);
        int maxfd = sock;

        if (select(maxfd+1, &rfds, NULL, NULL, NULL) < 0) 
        {
            if (errno == EINTR) 
            {
                continue;
            }
            die("select");
        }

        // incoming message from server
        if (FD_ISSET(sock, &rfds)) 
        {
            ssize_t n = recv(sock, line, sizeof(line)-1, 0);
            if (n <= 0) 
            {
                printf(COL_RED "Disconnected.\n" COL_RESET);
                break;
            }
            line[n] = '\0';
            printf("%s\n", line);
        }

        // user input
        if (FD_ISSET(0, &rfds)) 
        {
            if (!fgets(line, sizeof(line), stdin)) 
            {
                break;
            }
            line[strcspn(line,"\n")] = 0;

            if (strcasecmp(line,"quit")==0) 
            {
                break;
            }
            else if (strcasecmp(line,"LIST")==0) 
            {
                /* send LIST request */
                send(sock, "LIST", 4, 0);

                /* read server's LIST response until no more data arrives
                   (short timeout) so the full list is shown before prompting */
                {
                    char resp[MAX_LINE];
                    fd_set tmpfds;
                    struct timeval tv;
                    int rv;
                    do 
                    {
                        FD_ZERO(&tmpfds);
                        FD_SET(sock, &tmpfds);
                        tv.tv_sec = 0; tv.tv_usec = 200000; /* 200ms */
                        rv = select(sock+1, &tmpfds, NULL, NULL, &tv);
                        if (rv > 0 && FD_ISSET(sock, &tmpfds)) 
                        {
                            ssize_t rn = recv(sock, resp, sizeof(resp)-1, 0);
                            if (rn <= 0) break;
                            resp[rn] = '\0';
                            printf("%s", resp);
                        }
                    } 
                    while (rv > 0);
                }

                /* ---- Guided Chat after listing ---- */
                printf(COL_MAGENTA "Do you want to communicate with anyone? (yes/no): " COL_RESET);
                fflush(stdout);
                char ans[8];
                if (!fgets(ans,sizeof(ans),stdin)) 
                {
                    continue;
                }
                if (strncasecmp(ans,"yes",3)!=0) 
                {
                    continue;
                }
                // Enter the name of the client to chat with
                printf(COL_YELLOW "Enter exact name of client: " COL_RESET);
                fflush(stdout);
                char target[MAX_NAME];
                if (!fgets(target,sizeof(target),stdin)) 
                {
                    continue;
                }
                target[strcspn(target,"\n")] = 0;
                // Chat with the targeted client in a loop until /exit
                printf(COL_GREEN"\n--- Chat with %s ---\n" COL_RESET, target);
                printf(COL_WHITE"Type your messages (type /exit to leave chat)\n" COL_RESET);
                for (;;) 
                {
                    /* use select so we can receive incoming messages while
                       the user is typing */
                    fd_set chatfds;
                    FD_ZERO(&chatfds);
                    FD_SET(0, &chatfds);
                    FD_SET(sock, &chatfds);
                    int chatmax = sock;

                    if (select(chatmax + 1, &chatfds, NULL, NULL, NULL) < 0) 
                    {
                        if (errno == EINTR) 
                        {
                            continue;
                        }
                        die("select");
                    }

                    /* incoming from server */
                    if (FD_ISSET(sock, &chatfds)) 
                    {
                        ssize_t rn = recv(sock, line, sizeof(line)-1, 0);
                        if (rn <= 0) 
                        {
                            printf("\nDisconnected from server.\n");
                            goto end_chat;
                        }
                        line[rn] = '\0';
                        /* print incoming message on its own line */
                        printf("\r%s\n", line);
                        /* re-print prompt if user may be typing */
                        printf("You: ");
                        fflush(stdout);
                        continue;
                    }
                    /* user input */
                    if (FD_ISSET(0, &chatfds)) 
                    {
                        if (!fgets(line,sizeof(line),stdin)) 
                        {
                            break;
                        }
                        line[strcspn(line,"\n")] = 0;
                        if (strcmp(line,"/exit")==0) 
                        {
                            break;
                        }
                        char dm[MAX_LINE + MAX_NAME + 8];
                        /* server expects quoted full name: DM "full name" message */
                        snprintf(dm,sizeof(dm),"DM \"%s\" %s", target, line);
                        send(sock, dm, strlen(dm), 0);
                    }
                }
end_chat:
                printf(COL_RED "Left chat with %s ---\n" COL_RESET, target);
            } 
            else 
            {
                // any other text just sends as-is (optional)
                send(sock, line, strlen(line), 0);
            }
        }
    }
    close(sock);
    return 0;
}
