#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

// #define PORT 5550   /* Port that will be opened */ 
#define BACKLOG 1   /* Number of allowed connections */
#define BUFF_SIZE 1024
int checkMessage(char *buff)
{
    int result=0;
	int haveDigit = 0;
    int haveCharacter = 0;
	int haveSpecial = 0;
	int i=0;
	char *s= strdup(buff);
    for (i = 0; i < strlen(s); i++)
    {
        if (s[i] <= '9' && s[i] >= '0')
        {
            // digit[num1] = s[i];
            // printf("\nd: %c", s[i]);
            haveDigit =1;
			continue;
        }
        else if ((s[i] <= 'z' && s[i] >= 'a'))
        {
            // character[num2] = s[i];
            // printf("\nc: %c", s[i]);
            haveCharacter = 1;
			continue;
        }
        else if ((s[i] <= 'Z' && s[i] >= 'A'))
        {
            haveCharacter = 1;
			continue;
        }else{
			haveSpecial = 1;
		}
    }
	result = haveCharacter + haveDigit;
	if(haveSpecial){
		return 2;
	}
	if(result ==2){
		return 1;
	}
    return 0;
}
int modifyString(char *buff, char *character, char *digit)
{
    bzero(character, BUFF_SIZE);
    bzero(digit, BUFF_SIZE);
    int result = 0;
    int num1 = 0, num2 = 0;
	int haveDigit=0;
	int haveCharacter=0;
    int i;
    for (i = 0; i < strlen(buff); i++)
    {
        if (buff[i] <= '9' && buff[i] >= '0')
        {
            digit[num1] = buff[i];
            // printf("\nd: %c", buff[i]);
            num1++;
			haveDigit =1;
        }
        else if ((buff[i] <= 'z' && buff[i] >= 'a'))
        {
            character[num2] = buff[i];
            // printf("\nc: %c", buff[i]);
            num2++;
			haveCharacter=1;
        }
        else if ((buff[i] <= 'Z' && buff[i] >= 'A'))
        {
            character[num2] = buff[i];
            // printf("\nc: %c", buff[i]);
            num2++;
			haveCharacter=1;
        }
	}
    result = haveCharacter+haveDigit;
	if(result ==2){
		return 1;
	}else{
		return 0;
	}
}
int main(int argc, char* argv[])
{
	if(argc <1){
		printf("thieu param\n");
		return 0;
	}
	int PORT = atoi(argv[1]);
	int listen_sock, conn_sock; /* file descriptors */
	char recv_data[BUFF_SIZE];
	int bytes_sent, bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size;
	char digit[BUFF_SIZE], character[BUFF_SIZE];
	
	//Step 1: Construct a TCP socket to listen connection request
	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		return 0;
	}
	
	//Step 2: Bind address to socket
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;         
	server.sin_port = htons(PORT);   /* Remember htons() from "Conversions" section? =) */
	server.sin_addr.s_addr = htonl(INADDR_ANY);  /* INADDR_ANY puts your IP address automatically */   
	if(bind(listen_sock, (struct sockaddr*)&server, sizeof(server))==-1){ /* calls bind() */
		perror("\nError: ");
		return 0;
	}
	
	//Step 3: Listen request from client
	if(listen(listen_sock, BACKLOG) == -1){  /* calls listen() */
		perror("\nError: ");
		return 0;
	}
	
	//Step 4: Communicate with client
	while(1){
		//accept request
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock,( struct sockaddr *)&client, &sin_size)) == -1) 
			perror("\nError: ");
  
		printf("You got a connection from %s\n", inet_ntoa(client.sin_addr) ); /* prints client's IP */
		while(1){
			bzero(recv_data,BUFF_SIZE);
			bytes_received = recv(conn_sock, recv_data, BUFF_SIZE-1, 0); //blocking
			if (bytes_received <= 0){
				printf("\nConnection closed");
				break;
			}
			else{
				recv_data[bytes_received] = '\0';
				printf("\nReceive: %s ", recv_data);
				fflush(stdout);
			}
			if(strcmp(recv_data,"1")==0){
				while(1){
					//receives message from client
					bzero(recv_data,BUFF_SIZE);
					bytes_received = recv(conn_sock, recv_data, BUFF_SIZE-1, 0); //blocking
					// printf("%d: %s\n", strlen(recv_data),recv_data);
					if (strcmp(recv_data,"***")==0){
						printf("\nreturn menu");
						fflush(stdout);
						bytes_sent = send(conn_sock, "return menu", strlen("return menu"), 0); /* send to the client welcome message */
						if (bytes_sent <= 0){
							printf("\nConnection closed");
							break;
						}
						break;
					}
					else{
						recv_data[bytes_received] = '\0';
						printf("\nReceive: %s ", recv_data);
						fflush(stdout);
						// printf("%s\n", character);
						if(checkMessage(recv_data) == 1){
					
							modifyString(recv_data,character,digit);
							//echo to client
							bytes_sent = send(conn_sock, character, strlen(character), 0); /* send to the client welcome message */
							if (bytes_sent <= 0){
								printf("\nConnection closed");
								break;
							}

							bzero(recv_data,BUFF_SIZE);
							bytes_received = recv(conn_sock, recv_data, BUFF_SIZE-1, 0); //blocking
							if (strlen(recv_data)==0){
								printf("\nConnection closed");
								break;
							}
							// printf("\n");
							// recv_data[bytes_received] = '\0';
							// printf("\nReceive: %s ", recv_data);
							// fflush(stdout);

							bytes_sent = send(conn_sock, digit, strlen(digit), 0); /* send to the client welcome message */
							if (bytes_sent <= 0){
								printf("\nConnection closed");
								break;
							}
						}else if(checkMessage(recv_data) == 0){
							//echo to client
							bytes_sent = send(conn_sock, recv_data, strlen(recv_data), 0); /* send to the client welcome message */
							if (bytes_sent <= 0){
								printf("\nConnection closed");
								break;
							}
						}else{
							//echo to client
							bytes_sent = send(conn_sock, "error", strlen("error"), 0); /* send to the client welcome message */
							if (bytes_sent <= 0){
								printf("\nConnection closed");
								break;
							}
						}
					}
				}//end conversation
			}else{
				bytes_sent = send(conn_sock, "OK", strlen("OK"), 0); /* send to the client welcome message */
				if (bytes_sent <= 0){
					printf("\nConnection closed");
					break;
				}

				//receives message from client
				bzero(recv_data,BUFF_SIZE);
				bytes_received = recv(conn_sock, recv_data, BUFF_SIZE-1, 0); //blocking
				if (strlen(recv_data)==0){
					printf("\nConnection closed");
					break;
				}
				recv_data[bytes_received] = '\0';
				printf("\nReceive: %s ", recv_data);
				fflush(stdout);

				//echo to client
				bytes_sent = send(conn_sock, "done", strlen("done"), 0); /* send to the client welcome message */
				if (bytes_sent <= 0){
					printf("\nConnection closed");
					break;
				}

			}
			// //start conversation
			// while(1){
			// 	//receives message from client
			// 	bytes_received = recv(conn_sock, recv_data, BUFF_SIZE-1, 0); //blocking
			// 	if (bytes_received <= 0){
			// 		printf("\nConnection closed");
			// 		break;
			// 	}
			// 	else{
			// 		recv_data[bytes_received] = '\0';
			// 		printf("\nReceive: %s ", recv_data);
			// 		fflush(stdout);
			// 	}
				
			// 	//echo to client
			// 	bytes_sent = send(conn_sock, recv_data, bytes_received, 0); /* send to the client welcome message */
			// 	if (bytes_sent <= 0){
			// 		printf("\nConnection closed");
			// 		break;
			// 	}
			// }//end conversation	
		}
		close(conn_sock);
	}
	close(listen_sock);
	return 0;
}
