CC = gcc
OPTIMIZE = -O2
CFLAGS = $(OPTIMIZE) -pthread -g




server: userver.c
	@echo "building server..."
	@$(CC) $(CFLAGS) -o userver server.c
	@chmod +x control_server
	@echo "finished building server."

client: uclient.c
	@echo "building client..."
	@$(CC) $(CFLAGS) -o uclient client.c
	@chmod +x control_client
	@echo "finished building client."