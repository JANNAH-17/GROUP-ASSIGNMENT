FROM gcc:latest

WORKDIR /app

# Copy all files (including client.c)
COPY . .

# Compile the client code
RUN gcc -o c_client client.c

# Run the client
CMD ["./c_client"]