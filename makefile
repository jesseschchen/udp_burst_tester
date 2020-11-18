CC = gcc
OPTIMIZE = -O2
CFLAGS = $(OPTIMIZE) -pthread -g




server: userver.c
	@echo "building server..."
	@$(CC) $(CFLAGS) -o userver userver.c
	@chmod +x userver
	@echo "finished building server."

client: uclient.c
	@echo "building client..."
	@$(CC) $(CFLAGS) -o uclient uclient.c
	@chmod +x uclient
	@echo "finished building client."