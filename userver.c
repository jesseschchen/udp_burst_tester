#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>


int open_udp_port(struct sockaddr_in address) {
	int server_fd;

	if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket creation failure");
		exit(EXIT_FAILURE);
	}

	int enable = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	if (bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0) {
		perror("bind socket to address failed");
		exit(EXIT_FAILURE);
	}


	printf("opened UDP socket\n");

	return server_fd;
}


void recv_udp_burst(int server_fd, int burst_size, int per_packet_size, char* recv_buf) {
	for (int i = 0; i < burst_size; i++) {
		if (recvfrom(server_fd, recv_buf, per_packet_size, 0, NULL, NULL) < 0) {
			printf("recv() failed\n");
		}
		printf("recv packet:%i\n", i);
	}
}







void recv_udp_burst_loop(struct sockaddr_in address, int duration, int burst_interval, int burst_size, int per_packet_size){
	int num_bursts = (duration * 1000) / burst_interval;
	int server_fd = open_udp_port(address);

	char* recv_buf = malloc(per_packet_size);

	for (int i = 0; i < num_bursts; i++) {
		printf("receiving burst:%i\n", i);
		recv_udp_burst(server_fd, burst_size, per_packet_size, recv_buf);
	}


}



// ./userver <dst_ip_addr> <dst_port> <duration> <burst_interval> <burst_size>   || <per_packet_size>
int main(int argc, char const* argv[]) {
	char* dst_ip_addr = (char*) argv[1];
	int dst_port = atoi(argv[2]);
	int duration = atoi(argv[3]);
	int burst_interval = atoi(argv[4]);
	int burst_size = atoi(argv[5]);


	int per_packet_size = 1472;
	//int per_packet_size = atoi(argv[6]);

	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(dst_ip_addr);
	address.sin_port = htons(dst_port);


	recv_udp_burst_loop(address, duration, burst_interval, burst_size, per_packet_size);
}