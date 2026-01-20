#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h> 

#define PORT 8080
#define HOST_NAME "c-server" // Pastikan nama ini SAMA dengan 'service name' dalam docker-compose.yml

int main() {
    struct sockaddr_in serv_addr;
    struct hostent *server;

    printf("[C CLIENT] Client started for User 'Jannah'...\n");
    fflush(stdout);

    while(1) {
        // 1. Create Socket
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            printf("[C CLIENT] Error creating socket\n");
            sleep(5);
            continue;
        }

        server = gethostbyname(HOST_NAME);
        if (server == NULL) {
            printf("[C CLIENT] DNS Error: No such host '%s'\n", HOST_NAME);
            fflush(stdout);
            close(sock);
            sleep(10);
            continue;
        }

        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
        serv_addr.sin_port = htons(PORT);

        // 2. Connect to Server
        // printf("[C CLIENT] Connecting to %s...\n", HOST_NAME); // Optional log
        
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            printf("[C CLIENT] Connection Failed. Server might be down.\n");
        } else {
            // 3. Send Request
            // We specifically ask for Jannah's data
            char *msg = "Request_Points_Jannah";
            send(sock, msg, strlen(msg), 0);
            
            // 4. READ RESPONSE (This was missing before)
            char buffer[1024] = {0};
            int valread = read(sock, buffer, 1024);
            
            if (valread > 0) {
                printf("[C CLIENT] SUCCESS! Server Replied: %s\n", buffer);
            } else {
                printf("[C CLIENT] Connected, but received no data.\n");
            }
        }

        // 5. Clean up & Wait
        fflush(stdout); // Force logs to appear in Docker immediately
        close(sock);
        sleep(10); // Wait 10 seconds before next request
    }
    return 0;
}