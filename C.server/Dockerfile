FROM gcc:latest

WORKDIR /app

# --- UPDATE 1: Install MySQL Development Libraries ---
# This is required for <mysql/mysql.h> to work
RUN apt-get update && apt-get install -y default-libmysqlclient-dev

# Copy all files from the current directory to /app
COPY . .

# --- UPDATE 2: Compile with Linker Flags ---
# -lmysqlclient : Links the MySQL library
# -lpthread     : Links the threading library (required for background updates)
RUN gcc -o c_server main.c -lmysqlclient -lpthread

CMD ["./c_server"]