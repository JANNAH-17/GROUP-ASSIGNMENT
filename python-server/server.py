import socket
import time
import threading
import mysql.connector # REQUIRED: You must install this library
from datetime import datetime
import os

# --- CONFIGURATION ---
PORT = 8080
# We use the container name or service name from docker-compose
DB_HOST = "db"
DB_USER = "root"
DB_PASSWORD = "root"  # CHECK your docker-compose.yml for MYSQL_ROOT_PASSWORD
DB_NAME = "project_db"    # The new database we created
TARGET_USER = "Aizat"     # The user this Python server is responsible for

def get_db_connection():
    """Helper function to connect to the database"""
    return mysql.connector.connect(
        host=DB_HOST,
        user=DB_USER,
        password=DB_PASSWORD,
        database=DB_NAME
    )

def update_database_logic():
    """
    Background task: Updates 'Aizat' points every 30 seconds in MySQL.
    """
    print(f"[DB WORKER] Started for user: {TARGET_USER}")
    
    while True:
        # Wait 30 seconds before next update
        time.sleep(30)
        
        try:
            # 1. Connect to Database
            conn = get_db_connection()
            cursor = conn.cursor()

            # 2. Execute Update (Increment points)
            # Note: datetime_stamp updates automatically due to SQL table definition
            sql = "UPDATE scores SET points = points + 1 WHERE user = %s"
            cursor.execute(sql, (TARGET_USER,))
            conn.commit()
            
            # 3. Verify the update for logs
            cursor.execute("SELECT points, datetime_stamp FROM scores WHERE user = %s", (TARGET_USER,))
            result = cursor.fetchone()
            
            if result:
                new_points = result[0]
                timestamp = result[1]
                print("\n--- DATABASE UPDATE SUCCESS ---")
                print(f"User: {TARGET_USER}")
                print(f"New Points: {new_points}")
                print(f"Time: {timestamp}")
                print("-------------------------------")
            
            cursor.close()
            conn.close()
            
        except Exception as e:
            print(f"[ERROR] Database update failed: {e}")

def start_server():
    # 1. Setup Socket (IPv4, TCP)
    server_fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_fd.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_fd.bind(('0.0.0.0', PORT))
    server_fd.listen(3)

    print(f"[SERVER] Python Server (User: {TARGET_USER}) listening on port {PORT}...")

    # 2. Main Server Loop
    while True:
        try:
            client_socket, addr = server_fd.accept()
            with client_socket:
                # Receive request (not strictly used, but good for protocol)
                data = client_socket.recv(1024).decode('utf-8')
                
                # 3. Fetch LATEST points from Database
                points_to_send = "Error"
                try:
                    conn = get_db_connection()
                    cursor = conn.cursor()
                    cursor.execute("SELECT points FROM scores WHERE user = %s", (TARGET_USER,))
                    result = cursor.fetchone()
                    if result:
                        points_to_send = str(result[0])
                    conn.close()
                except Exception as e:
                    print(f"[ERROR] Could not fetch points: {e}")

                # 4. Send Response
                response = f"User: {TARGET_USER} | Points: {points_to_send}"
                client_socket.sendall(response.encode('utf-8'))
                print(f"[SERVER] Sent data to client: {response}")

        except Exception as e:
            print(f"[SERVER ERROR] {e}")

if __name__ == "__main__":
    # Start the background database updater
    updater_thread = threading.Thread(target=update_database_logic, daemon=True)
    updater_thread.start()

    # Start the main socket server
    start_server()