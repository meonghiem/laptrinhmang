#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5550
#define BUFF_SIZE 1024

void sig_chld(int signo){
	pid_t pid;
	int stat;
	
	/* Wait the child process terminate */
	while((pid = waitpid(-1, &stat, WNOHANG))>0)
		printf("\nChild %d terminated\n",pid);
}

int main(int argc, char * argv[]){
    if(argc !=5){
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
				// fflush(stdin);
                // printf("nhap username: ");
                // scanf("%s", username);
                strcpy(username,argv[3]);
                

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
                    
                    // printf("nhap password: ");
                    // fflush(stdin);
                    // scanf("%s",password);
                    strcpy(password,argv[4]);

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
						printf("username error\n");
                        exit(0);
					}else{

                    while(!strcmp(server_message, "not OK")){
                        printf("dang nhap that bai\n");
                        // bzero(password, 100);
                        // printf("%s\n",server_message);
                        // printf("nhap  lai password: ");
                        // fflush(stdin);
                        // scanf("%s",password);
                        // // if(sendto(socket_desc, password, strlen(password), 0,
                        // //     (struct sockaddr*)&server_addr, server_struct_length) < 0){
                        // //     printf("Unable to send message\n");
                        // //     return -1;
                        // // }
						// bytes_sent = send(client_sock,password,strlen(password),0);
						// if(bytes_sent < 0)perror("\nError: ");
                        // bzero(server_message, 2000);
                        // // if(recvfrom(socket_desc, server_message, sizeof(server_message), 0,
                        // //     (struct sockaddr*)&server_addr, &server_struct_length) < 0){
                        // //     printf("Error while receiving server's msg\n");
                        // //     return -1;
                        // // }
						// bytes_received = recv(client_sock, server_message, sizeof(server_message), 0);
						// if (bytes_received < 0)perror("\nError: ");

                        exit(0);
                    }
                    if(!strcmp(server_message,"Account is blocked")){
                        printf("%s", server_message);
                        exit(0);
                    }

                    printf("135line %s\n", server_message);
                    memset(server_message, '\0', sizeof(server_message));
                    // memset(username, '\0', sizeof(username));
                    // memset(password, '\0', sizeof(password));
                        printf("username: %s\n", username);
                        printf("password: %s\n", password);

					}

                    // show message after login successfully

                        bytes_sent = send(client_sock,"get full old message",strlen("get full old message"),0);
                        if(bytes_sent < 0)perror("\nError: ");
                        
                        bzero(server_message,2000);
                        bytes_received = recv(client_sock, server_message, sizeof(server_message), 0);
                        if (bytes_received < 0)perror("\nError: ");
                        server_message[strlen(server_message)-1]='\0';
                        printf("%s\n",server_message);
                    //end show message
                    
                    printf("end show message\n");
                    //send message to server and another client
                    signal(SIGCHLD, sig_chld);
                    pid_t pid = fork();
                    if(pid ==0){

                    char mes[1000];
                    while(1){
                            // show message after login successfully

                        bzero(server_message,2000);
                        bytes_received = recv(client_sock, server_message, sizeof(server_message), 0);
                        if (bytes_received < 0)perror("\nError: ");
                        // newsize = strlen(server_message);
                        // bytes_sent = send(client_sock,"OK",strlen("OK"),0);
                        // if(bytes_sent < 0)perror("\nError: ");
                        // server_message[strlen(server_message)-1]='\0';

                        sprintf(mes, "%s left room\n", username);
                        if(strncmp(server_message, mes, strlen(mes)) == 0) {
                            exit(0);
                        }

                        printf("%s\n",server_message);
                        //end show message
                        }

                         
                        

                        close(client_sock);
                        exit(0);
                    }

                        
                        
                    // FILE * f = fopen("message.txt", "a");
                    // int oldsize = ftell(f);
                    // fclose(f);
                    // int newsize;  
                        
                        while(1){
                            bzero(client_message,2000);
                        fflush(stdin);
                        scanf("%s", client_message);
                            
                        bytes_sent = send(client_sock,client_message,strlen(client_message),0);
                        if(bytes_sent < 0)perror("\nError: ");
                        // bzero(server_message,2000);
                        // bytes_received = recv(client_sock, server_message, sizeof(server_message), 0);
                        // if (bytes_received < 0)perror("\nError: ");
                        if(strcmp(client_message,"/exit") == 0){
                            sleep(1);
                            break;
                        }
                        }
                        

                    //end send message

                }else{

                    //memset server_message and username when username is not correct
                    memset(server_message, '\0', sizeof(server_message));
                    memset(username, '\0', sizeof(username));
                    printf("username is not correct.\n");
                    close(client_sock);
                    exit(0);
                }	
	
	//Step 4: Close socket
	close(client_sock);
	return 0;
}
