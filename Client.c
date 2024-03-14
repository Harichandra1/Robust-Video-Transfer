#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <time.h>
#include <pthread.h>

#define SERVER_PORT "4950" 
#define BUFFER_SIZE 500 

struct packet
{
	int sequence_no;
	int packet_size;
	char data[BUFFER_SIZE];
};

int socket_fd;
struct addrinfo serv_addr, *serv_info, *ptr; 
struct sockaddr_storage server_addr;
socklen_t server_addr_len = sizeof (struct sockaddr_storage);
int rv;

int data;
int no_of_bytes;
int in;
struct stat file_stat;
int fd;
off_t file_size;

struct packet packets[5]; 
int temp_seq_no = 1;
int no_of_acks;
int temp_ack;
int acks[5];
int no_of_packets = 5;

void* receiveAcks(void* vargp)
{
	for (int i = 0; i < no_of_packets; i++)
	{
		
	    RECEIVE:
		if((no_of_bytes = recvfrom(socket_fd, &temp_ack, sizeof(int), 0, (struct sockaddr*) &server_addr, &server_addr_len)) < 0)
		{
			perror("UDP Client: recvfrom");
			exit(1);
		} 
		
		if (acks[temp_ack] == 1)
			goto RECEIVE; 

		printf("Ack Received: %d\n", temp_ack);
		acks[temp_ack] = 1;
		no_of_acks++;

	}
		printf("\n\n");
    	return NULL;
}

int main(int argc, char* argv[]) {

	if (argc != 2)
	{
		printf("Error in UDP Client: usage: Client hostname\n");
		exit(1);
	}

	memset(&serv_addr, 0, sizeof serv_addr);
	serv_addr.ai_family = AF_UNSPEC;
	serv_addr.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo(argv[1], SERVER_PORT, &serv_addr, &serv_info)) != 0)
	{
		printf("UDP Client: getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	for(ptr = serv_info; ptr != NULL; ptr = ptr->ai_next)
	{
		if ((socket_fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol)) == -1)
		{
			printf("Error in UDP Client: socket");
			continue;
		}
		break;
	}

	if (ptr == NULL)
	{
		printf("Error in UDP Client: Failed to create socket\n");
		return 2;
	}

	pthread_t thread_id;

	struct timespec time1, time2;
	time1.tv_sec = 0;
	time1.tv_nsec = 300000000L;

	FILE * in = fopen("input_video.mp4","rb");

	if (in == NULL)
	{
		printf("Error in opening the video file.\n");
		return 0;
	}
	
	fd = fileno(in);
	fstat(fd, &file_stat);
	file_size = file_stat.st_size;
	printf("Size of Video File: %d bytes\n",(int) file_size);

	FILESIZE:
	if(sendto(socket_fd, &file_size, sizeof(off_t), 0, ptr->ai_addr, ptr->ai_addrlen) < 0)
		goto FILESIZE;

	data = 1;

	while (data > 0)
	{
		temp_seq_no = 0;
		for (int i = 0; i < no_of_packets; i++)
		{
			data = fread(packets[i].data, 1, BUFFER_SIZE, in);
			packets[i].sequence_no = temp_seq_no;
			packets[i].packet_size = data;
			temp_seq_no++;

           	if (data == 0)
           	{ 
				printf("End of file reached.\n");
                packets[i].packet_size = -1; 
                no_of_packets = i + 1; 
                break; 
            }
		}

		for (int i = 0; i < no_of_packets; i++)
		{
			printf("Sending packet %d\n", packets[i].sequence_no);
			if(sendto(socket_fd, &packets[i], sizeof(struct packet), 0, ptr->ai_addr, ptr->ai_addrlen) < 0)
			{
				printf("Error in UDP Client: sendto");
				exit(1);
			}            
		}

		for (int i = 0; i < no_of_packets; i++)
    		acks[i] = 0;

		no_of_acks = 0;

		pthread_create(&thread_id, NULL, receiveAcks, NULL);
                   
		nanosleep(&time1, &time2);
		
		// Send those packets whose ACKS have not been received yet
		RESEND:
		for (int i = 0; i < no_of_packets; i++)
		{
			if (acks[i] == 0)
			{
				printf("Sending missing packet: %d\n",packets[i].sequence_no);
				if(sendto(socket_fd, &packets[i], sizeof(struct packet), 0, ptr->ai_addr, ptr->ai_addrlen) < 0)
				{
					printf("Error in UDP Client: sendto");
					exit(1);
				}
			}
		}

		if (no_of_acks != no_of_packets) 
		{
            nanosleep(&time1, &time2);
			goto RESEND;
		}

		pthread_join(thread_id, NULL);
	}

	printf("\nFile transfer completed successfully!\n");
	close(socket_fd); // close the socket
	return 0;
}
