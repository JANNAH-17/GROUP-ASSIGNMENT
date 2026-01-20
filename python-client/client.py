import socket
import time

# --- CONFIGURATION ---
# IMPORTANT: This must match the SERVICE name in docker-compose.yml
# NOT the container name. usually it is 'python-server' or 'server-python'
SERVER_HOST = 'python-server' 
SERVER_PORT = 8080

def start_client():
    print(f"[CLIENT] Client for User 'Aizat' started...", flush=True)

    while True:
        # [cite_start]1. Create Socket (IPv4, TCP) [cite: 38]
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
        # Set timeout to prevent hanging
        sock.settimeout(5)

        try:
            print(f"[CLIENT] Connecting to {SERVER_HOST}:{SERVER_PORT}...", flush=True)
            
            # 2. Connect to Server
            # Docker handles the DNS resolution of 'python-server'
            sock.connect((SERVER_HOST, SERVER_PORT))
            
            print("[CLIENT] Connected!", flush=True)

            # 3. Send Request
            # We explicitly ask for Aizat's data, though the server is currently configured to give it anyway.
            message = "Request_Points_Aizat"
            sock.sendall(message.encode())

            # 4. Receive Response
            # The server will query the database and send back the points
            response = sock.recv(1024).decode()
            print(f"[CLIENT] SERVER RESPONSE: {response}", flush=True)

        except socket.gaierror:
            print(f"[ERROR] DNS Error: Could not resolve hostname '{SERVER_HOST}'. Check docker-compose service name.", flush=True)
        except ConnectionRefusedError:
            print("[ERROR] Connection Refused. Is the server running?", flush=True)
        except socket.timeout:
            print("[ERROR] Connection Timed Out.", flush=True)
        except Exception as e:
            print(f"[ERROR] An unexpected error occurred: {e}", flush=True)
        
        finally:
            # 5. Close and Wait
            sock.close()
            print("[CLIENT] Waiting 10 seconds before next request...\n", flush=True)
            time.sleep(10)

if __name__ == "__main__":
    start_client()