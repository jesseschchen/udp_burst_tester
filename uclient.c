#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>




char* read_random_bytes(int num_bytes) {
	char* buffer = malloc(num_bytes);
	FILE* fp = fopen("/dev/urandom", "r");
	int bytes_read = fread(buffer, 1, num_bytes, fp);


	fclose(fp);
	return buffer;
}


char* create_message(int num_bytes, char* message_id, char* burst_id, char* random_bytes) {
	char* message = random_bytes;

	//char* syn = "SYN\0";
	//char* fin = "FIN\0";
	char* syn = "SYN*";
	char* fin = "FIN*";

	memcpy(message, syn, 4);
	memcpy(&message[4], message_id, 4);

	memcpy(&message[num_bytes-8], fin, 4);
	memcpy(&message[num_bytes-4], message_id, 4);

	//printf("%s\n", message);

	return message;
}



int open_udp_port() {
	int client_fd;
	struct sockaddr_in address;

	if ((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	} 

	printf("opened UDP socket\n");
	return client_fd;
}


// creates a array of char arrays
char** create_burst_packets(int burst_size, int per_packet_size) {
	// malloc pointers for each packet
	char** burst_packets = malloc(burst_size * sizeof(char*));
	for (int i = 0; i < burst_size; i++) {
		burst_packets[i] = (char*)malloc(per_packet_size);
		//strcpy(burst_packets[i], "123456789");
	}
	return burst_packets;
}


// burst_id = 4 bytes long
char** modify_burst_packets(char** burst_packets, int burst_size, int per_packet_size, char* burst_id) {
	for (int i = 0; i < burst_size; i++) {
		printf("packet_id: %i:%i\n", (int)*burst_id, i);

		memcpy(burst_packets[i], burst_id, 4);  // burst_id = first 4 bytes
		memcpy(&burst_packets[i][4], (char*)&i, 4);  // burst_packet_num = 2nd 4 bytes
	}
	return burst_packets;
}


int send_udp_burst(int client_fd, struct sockaddr_in address, int burst_size, int per_packet_size, char** burst_packets) {
	for (int i = 0; i < burst_size; i++) {
		if (sendto(client_fd, burst_packets[i], per_packet_size, 0, (struct sockaddr*) &address, sizeof(address)) < 0) {
			printf("failed to send\n");
		}
	}
}

// duration = seconds
// burst_interval = milliseconds
void send_udp_burst_loop(struct sockaddr_in address, int duration, int burst_interval, int burst_size, int per_packet_size) {
	int num_bursts = (duration * 1000)/burst_interval;
	int client_fd = open_udp_port();
	char** burst_packets = create_burst_packets(burst_size, per_packet_size);

	for (int i = 0; i < num_bursts; i++) {
		modify_burst_packets(burst_packets, burst_size, per_packet_size, (char*)&i);

		send_udp_burst(client_fd, address, burst_size, per_packet_size, burst_packets);
		printf("burst sent\n");
	}

}

// ./uclient <dst_ip_addr> <dst_port> <duration> <burst_interval> <burst_size>
int main(int argc, char const* argv[]) {
	char* dst_ip_addr = (char*)argv[1];
	int dst_port = atoi(argv[2]);
	int duration = atoi(argv[3]);
	int burst_interval = atoi(argv[4]);
	int burst_size = atoi(argv[5]);


	int per_packet_size = 10;


	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(dst_ip_addr);
	address.sin_port = htons(dst_port);

	send_udp_burst_loop(address, duration, burst_interval, burst_size, per_packet_size);

}	


