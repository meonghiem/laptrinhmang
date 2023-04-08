#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5550
#define BUFF_SIZE 1024

int main(int argc, char * argv[]){
    if(argc !=3){
        printf("sai input\n");
        return 0;
    }

	int client_sock;
	char buff[BUFF_SIZE + 1];
	struct sockaddr_in server_addr; /* server's address information */
	int msg_len, bytes_sent, bytes_received;
	
	//Step 1: Construct socket
	client_sock = socket(AF_INET,SOCK_STREAM,0);
	
	//Step 2: Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	
	//Step 3: Request to connect server
	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		printf("\nError!Can not connect to sever! Client exit imediately! ");
		return 0;
	}
		
	//Step 4: Communicate with server			
	
	//send message
	// printf("\nInsert string to send:");
	// memset(buff,'\0',(strlen(buff)+1));
	// fgets(buff, BUFF_SIZE, stdin);		
	// msg_len = strlen(buff);
		
	// bytes_sent = send(client_sock, buff, msg_len, 0);
	// if(bytes_sent < 0)
	// 	perror("\nError: ");
	
	// //receive echo reply
	// bytes_received = recv(client_sock, buff, BUFF_SIZE, 0);
	// if (bytes_received < 0)
	// 		perror("\nError: ");
	// else if (bytes_received == 0)
	// 		printf("Connection closed.\n");
		
	// buff[bytes_received] = '\0';
	// printf("Reply from server: %s", buff);

	// login(client_sock);

	// char buff[BUFF_SIZE + 1];
	char server_message[2000], client_message[2000];
	char username[100], password[100];
	 memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));
	// int msg_len, bytes_sent, bytes_received;
				fflush(stdin);
                printf("nhap username: ");
                scanf("%s", username);
                

                // Send the message to server:
                bytes_sent = send(client_sock, username, strlen(username), 0);
				if(bytes_sent < 0)perror("\nError: ");

                // if(recvfrom(socket_desc, server_message, sizeof(server_message), 0,
                //     (struct sockaddr*)&server_addr, &server_struct_length) < 0){
                //     printf("Error while receiving server's msg\n");
                //     return -1;
                // }
				bytes_received = recv(client_sock, server_message, sizeof(server_message), 0);
				if (bytes_received < 0)perror("\nError: ");
                if(strcmp(server_message, "Insert password")==0){

                    //memset server_message
                    
                    printf("nhap password: ");
                    fflush(stdin);
                    scanf("%s",password);

                    // if(sendto(socket_desc, password, strlen(password), 0,
                    //     (struct sockaddr*)&server_addr, server_struct_length) < 0){
                    //     printf("Unable to send message\n");
                    //     return -1;
                    // }
					bytes_sent = send(client_sock,password,strlen(password),0);
					if(bytes_sent < 0)perror("\nError: ");
					
                    
                    bzero(server_message, 2000);
                    // if(recvfrom(socket_desc, server_message, sizeof(server_message), 0,
                    //     (struct sockaddr*)&server_addr, &server_struct_length) < 0){
                    //     printf("Error while receiving server's msg\n");
                    //     return -1;
                    // }
                    // printf("%s\n",server_message);
					bytes_received = recv(client_sock, server_message, sizeof(server_message), 0);
					if (bytes_received < 0)perror("\nError: ");
					if(strcmp(server_message, "username error")==0){
						printf("usernaem error\n");
					}else{

                    while(!strcmp(server_message, "not OK")){
                        bzero(password, 100);
                        printf("%s\n",server_message);
                        printf("nhap  lai password: ");
                        fflush(stdin);
                        scanf("%s",password);
                        // if(sendto(socket_desc, password, strlen(password), 0,
                        //     (struct sockaddr*)&server_addr, server_struct_length) < 0){
                        //     printf("Unable to send message\n");
                        //     return -1;
                        // }
						bytes_sent = send(client_sock,password,strlen(password),0);
						if(bytes_sent < 0)perror("\nError: ");
                        bzero(server_message, 2000);
                        // if(recvfrom(socket_desc, server_message, sizeof(server_message), 0,
                        //     (struct sockaddr*)&server_addr, &server_struct_length) < 0){
                        //     printf("Error while receiving server's msg\n");
                        //     return -1;
                        // }
						bytes_received = recv(client_sock, server_message, sizeof(server_message), 0);
						if (bytes_received < 0)perror("\nError: ");
                    }
                    printf("%s\n", server_message);
                    memset(server_message, '\0', sizeof(server_message));
                    memset(username, '\0', sizeof(username));
                    memset(password, '\0', sizeof(password));
                    
					}
                }else{

                    //memset server_message and username when username is not correct
                    memset(server_message, '\0', sizeof(server_message));
                    memset(username, '\0', sizeof(username));
                    printf("username is not correct.\n");
                }	
	
	//Step 4: Close socket
	close(client_sock);
	return 0;
}

void login(int client_sock){
	char buff[BUFF_SIZE + 1];
	char server_message[2000], client_message[2000];
	char username[100], password[100];
	 memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));
	int msg_len, bytes_sent, bytes_received;
				fflush(stdin);
                printf("nhap username: ");
                scanf("%s", username);
                

                // Send the message to server:
                bytes_sent = send(client_sock, username, strlen(username), 0);
				if(bytes_sent < 0)perror("\nError: ");

                // if(recvfrom(socket_desc, server_message, sizeof(server_message), 0,
                //     (struct sockaddr*)&server_addr, &server_struct_length) < 0){
                //     printf("Error while receiving server's msg\n");
                //     return -1;
                // }
				bytes_received = recv(client_sock, server_message, sizeof(server_message), 0);
				if (bytes_received < 0)perror("\nError: ");
                if(strcmp(server_message, "Insert password")==0){

                    //memset server_message
                    
                    printf("nhap password: ");
                    fflush(stdin);
                    scanf("%s",password);

                    // if(sendto(socket_desc, password, strlen(password), 0,
                    //     (struct sockaddr*)&server_addr, server_struct_length) < 0){
                    //     printf("Unable to send message\n");
                    //     return -1;
                    // }
					bytes_sent = send(client_sock,password,strlen(password),0);
					if(bytes_sent < 0)perror("\nError: ");
					
                    
                    bzero(server_message, 2000);
                    // if(recvfrom(socket_desc, server_message, sizeof(server_message), 0,
                    //     (struct sockaddr*)&server_addr, &server_struct_length) < 0){
                    //     printf("Error while receiving server's msg\n");
                    //     return -1;
                    // }
                    // printf("%s\n",server_message);
					bytes_received = recv(client_sock, server_message, sizeof(server_message), 0);
					if (bytes_received < 0)perror("\nError: ");
                    while(!strcmp(server_message, "not OK")){
                        bzero(password, 100);
                        printf("%s\n",server_message);
                        printf("nhap  lai password: ");
                        fflush(stdin);
                        scanf("%s",password);
                        // if(sendto(socket_desc, password, strlen(password), 0,
                        //     (struct sockaddr*)&server_addr, server_struct_length) < 0){
                        //     printf("Unable to send message\n");
                        //     return -1;
                        // }
						bytes_sent = send(client_sock,password,strlen(password),0);
						if(bytes_sent < 0)perror("\nError: ");
                        bzero(server_message, 2000);
                        // if(recvfrom(socket_desc, server_message, sizeof(server_message), 0,
                        //     (struct sockaddr*)&server_addr, &server_struct_length) < 0){
                        //     printf("Error while receiving server's msg\n");
                        //     return -1;
                        // }
						bytes_received = recv(client_sock, server_message, sizeof(server_message), 0);
						if (bytes_received < 0)perror("\nError: ");
                    }
                    printf("%s\n", server_message);
                    memset(server_message, '\0', sizeof(server_message));
                    memset(username, '\0', sizeof(username));
                    memset(password, '\0', sizeof(password));
                    
                }else{

                    //memset server_message and username when username is not correct
                    memset(server_message, '\0', sizeof(server_message));
                    memset(username, '\0', sizeof(username));
                    printf("username is not correct.\n");
                }
}