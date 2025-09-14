# MY_CHATBOT_APP

## Overview

MY_CHATBOT_APP is a simple multi-client chat application written in C, enabling real-time communication between clients through a central server. It demonstrates fundamental concepts of network programming using TCP sockets and select-based multiplexing, providing features like client registration, broadcasting, and direct messaging.


## Features

- Multi-client support with a limit of 32 clients
- Client registration by username
- Broadcast messages to all connected clients
- Direct messaging (DM) between registered clients
- Command to list all connected clients
- Graceful handling of client disconnections and server shutdown
- Color-coded console output for easy monitoring


## Project Structure

| File                      | Description                                          |
|---------------------------|------------------------------------------------------|
| `SERVER/server_main.c`    | Main server program handling connections and events |
| `SERVER/server_helper.c`  | Helper functions for client management and messaging |
| `SERVER/server_helper.h`  | Header file for server helper functions              |
| `SERVER/server.h`         | Server constants, data structures, and includes      |
| `CLIENT/client_main.c`    | Main client program for connecting and interacting   |
| `CLIENT/client_helper.c`  | Client-side helper functions                          |
| `CLIENT/client_helper.h`  | Header for client helper functions                    |
| `CLIENT/client.h`         | Client constants, includes, and color codes           |


## Prerequisites

- POSIX-compliant OS (Linux, macOS)
- GCC or compatible C compiler
- Basic networking setup allowing TCP connections on chosen port
