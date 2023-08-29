#include "main.h"

#define NUM_CLIENTS 1

#define SERVER_IP "127.0.0.1"
#define NUM_SERVER_PORTS 1
const int TCP_PORTS[NUM_SERVER_PORTS] = { 4000 };

#define MAX_MSG_SIZE 1024

char* LOG_FILE = "C:\\Users\\avons\\Code\\Anatomic\\TCP_Client\\logs\\TCP_Client.log";

// Connect to the TCP Server, send the message, log the result
DWORD WINAPI test_client(LPVOID param) {
    uint64_t start_time, end_time;
    SOCKET clientSocket = init_client(SERVER_IP, TCP_PORTS[0]);  // connecting to the first port in the TCP_PORTS array

    char buffer[MAX_MSG_SIZE];
    for (int i = 0; i < 5; i++) {
        uint64_t request = URI_GET_TIME;
        start_time = GetTickCount64();
        send_to_server(clientSocket, (const char*)&request, sizeof(request)); // Send encoded 64-bit request


        uint64_t response_data = 0; // Renamed the variable for clarity
        MessageType response_type;

        int timeoutCounter = 0;
        while (true) {  // Loop until confirmation or timeout
            int bytesRead = read_message_from_server(clientSocket, (char*)&response_data);

            if (response_data != 0) {
                interpret_message(response_data, &response_type);
                if (bytesRead == sizeof(uint64_t) && response_type == CONFIRM_MESSAGE) {
                    break;  // Break out of the loop if confirmation is received
                }

                if (++timeoutCounter >= 10) {  // This will give a total of 10*SOCKET_TIMEOUT_MS time for waiting for the confirmation.
                    write_log("Did not receive a proper confirmation from the server.");
                    break;
                }
            }
        }

        if (response_data) {
            int totalBytesRead = 0;
            int timeoutCounter = 0;

            while (totalBytesRead < sizeof(uint64_t) && timeoutCounter < 10) {
                int bytesRead = read_message_from_server(clientSocket, (char*)&response_data);

                if (totalBytesRead == sizeof(uint64_t)) {
                    break;
                }
                if (bytesRead > 0) {
                    totalBytesRead += bytesRead;
                }
                else {
                    timeoutCounter++;
                }
            }

            if (totalBytesRead == sizeof(uint64_t)) {
                end_time = GetTickCount64();
                uint64_t delay = end_time - start_time;

                char msg_buffer[MAX_LOG_SIZE];
                snprintf(msg_buffer, sizeof(msg_buffer), "TCP Client: Received response '%llu' with delay of %llu ms", response_data, delay);
                write_log(msg_buffer);
            }
            else {
                write_log("Received incorrect number of bits for the response.");
            }
        }

    }


    cleanup_client(clientSocket);
    return 0;
}

int main() {
    init_logger(LOG_FILE);

    // Initiate client test threads
    HANDLE clientThreads[NUM_CLIENTS] = { 0 };
    HANDLE clientThread;
    clientThread = CreateThread(NULL, 0, test_client, NULL, 0, NULL);
    if (clientThread == NULL) {
        write_log("Error creating client test thread.");
    }
    clientThreads[0] = clientThread;

    // Wait for all threads to exit
    for (int i = 0; i < NUM_CLIENTS; i++) {
        if (clientThreads[i]) {
            WaitForSingleObject(clientThreads[i], INFINITE);
            CloseHandle(clientThreads[i]);
        }
    }

    close_logger();
    return 0;
}
