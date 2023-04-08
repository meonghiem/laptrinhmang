#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

// #define SERVER_ADDR "127.0.0.1"
// #define SERVER_PORT 5550
#define BUFF_SIZE 8192
void readFile(char *file, char *buff){
    FILE* ptr;
    char ch;
    int i=0;
 
    // Opening file in reading mode
    ptr = fopen("text.txt", "r");
 
    if (NULL == ptr) {
        printf("file can't be opened \n");
        return;
    }
 
    
 
    // Printing what is written in file
    // character by character using loop.
    do {
        buff[i] = fgetc(ptr);
        // printf("%c", ch);
        i++;
 
        // Checking if character is not EOF.
        // If it is EOF stop eading.
    } while (!feof(ptr));
 
    // Closing the file
    fclose(ptr);
}
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
void clearStdin(void){  

    int c;

    while(( c = getchar() ) != '\n' && ( c != EOF ));
}
int main(int argc, int * argv[]){
	if(argc <2){
		printf("thieu param\n");
		return 0;
	}
	int SERVER_PORT = atoi(argv[2]);
	char * SERVER_ADDR = argv[1];
	int client_sock;
	char buff[BUFF_SIZE];
	struct sockaddr_in server_addr; /* server's address information */
	int msg_len, bytes_sent, bytes_received;
	
	//Step 1: Construct socket
	client_sock = socket(AF_INET,SOCK_STREAM,0);
	
	//Step 2: Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	
	//Step 3: Request to connect server
	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		printf("\nError!Can not connect to sever! Client exit imediately! ");
		return 0;
	}
	
	int choice;
	//Step 4: Communicate with server			
	while(1){

		printf("1. send string\n");
		printf("2. send content of file\n");
		printf(" =============== \n");

		printf("nhap choice: ");
		fflush(stdin);

		fscanf(stdin,"%d", &choice);
		bzero(buff,BUFF_SIZE);
		snprintf(buff,BUFF_SIZE,"%d", choice);
		bytes_sent = send(client_sock, &buff, strlen(buff), 0);
		switch(choice){
			case 1:
				fgets(buff, BUFF_SIZE, stdin);
				while(1){
					//send message
					
					printf("\nInsert string to send:");
					// memset(buff,'\0',(strlen(buff)));
					bzero(buff, BUFF_SIZE);
					
					// clearStdin();
					fgets(buff, BUFF_SIZE, stdin);
					buff[strlen(buff)-1]='\0';
					// scanf("%s", buff);
					// buff[strlen(buff)-1]='\0';
					msg_len = strlen(buff);
					if (msg_len == 0){
						bytes_sent = send(client_sock, "***", strlen("***"), 0);
						if(bytes_sent <= 0){
							printf("\nConnection closed!\n");
							break;
						}

						bzero(buff,BUFF_SIZE);
						bytes_received = recv(client_sock, buff, BUFF_SIZE-1, 0);
						if(bytes_received <= 0){
							printf("\nError!Cannot receive data from sever!\n");
							break;
						}
						buff[bytes_received] = '\0';
						printf("Reply from server : %s\n", buff);
						break;
					}
					
					bytes_sent = send(client_sock, buff, msg_len, 0);
					// printf("%d", bytes_sent);
					if(bytes_sent <= 0){
						printf("\nConnection closed!\n");
						break;
					}
					
					if(checkMessage(buff) ==1){
						//receive echo reply
						bytes_received = recv(client_sock, buff, BUFF_SIZE-1, 0);
						if(bytes_received <= 0){
							printf("\nError!Cannot receive data from sever!\n");
							break;
						}
						buff[bytes_received] = '\0';
						printf("Reply from server character: %s\n", buff);

						bytes_sent = send(client_sock, "OK", strlen("OK"), 0);
						if(bytes_sent <= 0){
							printf("\nConnection closed!\n");
							break;
						}
						
						bytes_received = recv(client_sock, buff, BUFF_SIZE-1, 0);
						if(bytes_received <= 0){
							printf("\nError!Cannot receive data from sever!\n");
							break;
						}
						buff[bytes_received] = '\0';
						printf("Reply from server digital: %s\n", buff);
					}else{
						bytes_received = recv(client_sock, buff, BUFF_SIZE-1, 0);
						if(bytes_received <= 0){
							printf("\nError!Cannot receive data from sever!\n");
							break;
						}
						buff[bytes_received] = '\0';
						printf("Reply from server: %s\n", buff);
					}
				}
				break;
			case 2: 

				bytes_received = recv(client_sock, buff, BUFF_SIZE-1, 0);
				if(bytes_received <= 0){
					printf("\nError!Cannot receive data from sever!\n");
					break;
				}
				buff[bytes_received] = '\0';
				printf("Reply from server: %s\n", buff);

				bzero(buff, BUFF_SIZE);
				readFile("text.txt",buff);
				// printf("%s\n", buff);
				msg_len = strlen(buff);
				if (msg_len == 0) break;
				bytes_sent = send(client_sock, buff, msg_len, 0);
				if(bytes_sent <= 0){
					printf("\nConnection closed!\n");
					break;
				}

				bytes_received = recv(client_sock, buff, BUFF_SIZE-1, 0);
				if(bytes_received <= 0){
					printf("\nError!Cannot receive data from sever!\n");
					break;
				}
				buff[bytes_received] = '\0';
				printf("Reply from server: %s\n", buff);

				break;
			default: break;
		}		
	}
	
	//Step 4: Close socket
	close(client_sock);
	return 0;
}
