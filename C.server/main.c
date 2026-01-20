#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <mysql/mysql.h>

#define PORT 8080
#define DB_HOST "db"
#define DB_USER "root"
#define DB_PASS "root" 
#define DB_NAME "project_db"

// Helper to print MySQL errors
void finish_with_error(MYSQL *con) {
    fprintf(stderr, "[DB ERROR] %s\n", mysql_error(con));
    mysql_close(con);
}

// Helper to get a database connection
MYSQL* get_db_connection() {
    MYSQL *con = mysql_init(NULL);
    if (con == NULL) {
        fprintf(stderr, "mysql_init() failed\n");
        return NULL;
    }
    if (mysql_real_connect(con, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0) == NULL) {
        finish_with_error(con);
        return NULL;
    }
    return con;
}

// --- THREAD 1: BACKGROUND UPDATE LOOP (Every 30s) ---
void *update_database_loop(void *arg) {
    printf("[DB WORKER] Background thread started for User: Jannah\n");
    
    while(1) {
        sleep(30); // Wait 30 seconds

        MYSQL *con = get_db_connection();
        if (con) {
            // Requirement: Points must increase every time updated
            if (mysql_query(con, "UPDATE scores SET points = points + 1 WHERE user = 'Jannah'")) {
                fprintf(stderr, "[DB UPDATE FAIL] %s\n", mysql_error(con));
            } else {
                // Optional: Fetch current points just to show in logs
                if (!mysql_query(con, "SELECT points, datetime_stamp FROM scores WHERE user = 'Jannah'")) {
                    MYSQL_RES *result = mysql_store_result(con);
                    MYSQL_ROW row = mysql_fetch_row(result);
                    if (row) {
                         printf("\n--- DATABASE UPDATE SUCCESS ---\n");
                         printf("User: Jannah\nPoints: %s\nTime: %s\n", row[0], row[1]);
                         printf("-------------------------------\n");
                         fflush(stdout);
                    }
                    mysql_free_result(result);
                }
            }
            mysql_close(con);
        }
    }
    return NULL;
}

// --- MAIN THREAD: SOCKET SERVER ---
int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    pthread_t thread_id;

    // 1. Start the Background Update Thread
    if(pthread_create(&thread_id, NULL, update_database_loop, NULL) != 0) {
        printf("Failed to create background thread\n");
        return 1;
    }

    // 2. Setup Socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    
    listen(server_fd, 3);
    printf("[SERVER] C Server listening on port %d...\n", PORT);
    fflush(stdout);

    // 3. Main Listener Loop
    while(1) {
        printf("[SERVER] Waiting for client connection...\n");
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        
        if (new_socket >= 0) {
            char buffer[1024] = {0};
            read(new_socket, buffer, 1024);
            printf("[SERVER] Client sent: %s\n", buffer);

            // Fetch latest points from Database to send to Client
            int points = -1;
            MYSQL *con = get_db_connection();
            if (con) {
                if (mysql_query(con, "SELECT points FROM scores WHERE user = 'Jannah'")) {
                    fprintf(stderr, "%s\n", mysql_error(con));
                } else {
                    MYSQL_RES *result = mysql_store_result(con);
                    MYSQL_ROW row = mysql_fetch_row(result);
                    if (row) points = atoi(row[0]);
                    mysql_free_result(result);
                }
                mysql_close(con);
            }

            // Send Response
            char response[100];
            if (points != -1)
                sprintf(response, "User: Jannah | Points: %d", points);
            else
                sprintf(response, "Error fetching data");

            send(new_socket, response, strlen(response), 0);
            close(new_socket);
            printf("[SERVER] Response sent & connection closed.\n");
        }
    }
    return 0;
}