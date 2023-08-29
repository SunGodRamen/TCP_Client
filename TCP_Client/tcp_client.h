// TCP_Client.h

#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H
#pragma once

#include <stdio.h>
#include <stdint.h>
#include <winsock2.h>
#include "logger.h"

int read_message_from_server(SOCKET socket, char* buffer);

// Initialize the client
SOCKET init_client(const char* serverIP, uint16_t port);

// Send data to the server
void send_to_server(SOCKET serverSocket, const char* data, int dataLength);

// Cleanup and close the client
void cleanup_client(SOCKET serverSocket);

#endif
