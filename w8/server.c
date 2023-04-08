#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <pthread.h>

#include <string.h>
#include <ctype.h>
// #include "fields.h"

// #define PORT 5550  /* Port that will be opened */
#define BACKLOG 20 /* Number of allowed connections */
#define BUFF_SIZE 1024

int count = 0;

typedef struct client
{

	int index;
	int sockID;
} client;

client Client[BACKLOG];
pthread_t thread[BACKLOG];

/* Receive and echo message to client */
void *echo(void *);

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("Please input portnumber!\n");
		exit(1);
	}
	int listenfd, *connfd;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size;
	pthread_t tid;

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{ /* calls socket() */
		perror("\nError: ");
		return 0;
	}
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(argv[1]));
	server.sin_addr.s_addr = htonl(INADDR_ANY); /* INADDR_ANY puts your IP address automatically */

	if (bind(listenfd, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		perror("\nError: ");
		return 0;
	}

	if (listen(listenfd, BACKLOG) == -1)
	{
		perror("\nError: ");
		return 0;
	}

	sin_size = sizeof(struct sockaddr_in);
	while (1)
	{
		// connfd = malloc(sizeof(int));
		if ((Client[count].sockID = accept(listenfd, (struct sockaddr *)&client, &sin_size)) == -1)
			perror("\nError: ");

		printf("You got a connection from %s\n", inet_ntoa(client.sin_addr)); /* prints client's IP */

		/* For each client, spawns a thread, and the thread handles the new client */
		// pthread_create(&tid, NULL, &echo, connfd);
		Client[count].index = count;
		pthread_create(&thread[count], NULL, &echo, (void *)&Client[count]);

		count++;
	}
	// int i = 0;
	// for (i = 0; i < count; i++)
	// 	pthread_join(thread[i], NULL);
	close(listenfd);
	return 0;
}

void *echo(void *arg)
{
	pthread_detach(pthread_self());
	client *clientDetail = (client *)arg;
	int index = clientDetail->index;
	int connfd = clientDetail->sockID;

	int bytes_sent, bytes_received;
	char buff[BUFF_SIZE + 1];
	printf("Client %d connected.\n", index + 1);

	bytes_received = recv(connfd, buff, BUFF_SIZE, 0); // blocking
	if (bytes_received < 0)
		perror("\nError: ");
	else if (bytes_received == 0)
		printf("Connection closed.");
	char filename[50];
	sprintf(filename, "log_client%d.txt\n", count);
	FILE *fp = fopen(filename, "w");

	if (fp == NULL)
	{
		printf("Error!");
		return NULL;
	}

	fprintf(fp, "%s", buff);
	fclose(fp);
	close(connfd);

	return NULL;
}
