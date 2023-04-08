
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>
#include <ctype.h>

void printMess(char *message){
	char digit[1000];
    char alpha[1000];
    // bzero(digit,1000);
    // bzero(alpha, 1000);
    memset(digit, '\0', sizeof(digit));
    memset(alpha, '\0', sizeof(alpha));
	int num1 = 0, num2 = 0;
	int i;
	for(i = 0; i < strlen(message); i++){
		if(message[i] <= '9' && message[i] >= '0'){
			digit[num1] = message[i];
			num1++;
		}else if((message[i] <= 'z' && message[i] >= 'a')){
			alpha[num2] = message[i];
			num2++;
		}else if((message[i] <= 'Z' && message[i] >= 'A')){
			alpha[num2] = message[i];
			num2++;
		}	
	}
    printf("%s\n",alpha);
    printf("%s\n", digit);
	return;
}
int main(int argc, char * argv[]){
    if(argc !=3){
        printf("sai input\n");
        return 0;
    }
    int socket_desc;
    struct sockaddr_in server_addr;
    char server_message[2000], client_message[2000];
    int server_struct_length = sizeof(struct sockaddr);
    
    // Clean buffers:
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));
    
    // Create socket:
    socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    if(socket_desc < 0){
        printf("Error while creating socket\n");
        return -1;
    }
    printf("Socket created successfully\n");
    // printf("%d", atoi(argv[1]))
    // Set port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    int choice;
    int count =0;
    char username[100], password[100];
    while(1){
        printf("USER MANAGEMENT PROGRAM\n");
        printf("-----------------------------------\n");
        printf("2. Sign in\n");
        printf("3. Send data\n");
        printf("6. Get data\n");
        printf("4. Sign out\n");
        printf("5. Exit\n");
        printf("Your choice (1-4, other to quit):\n");
        memset(server_message, '\0', sizeof(server_message));
        memset(client_message, '\0', sizeof(client_message));
        printf("nhap choice: ");
        scanf("%d", &choice);
        switch(choice){
            case 2:

                if(sendto(socket_desc, "Sign in", strlen("Sign in"), 0,
                    (struct sockaddr*)&server_addr, server_struct_length) < 0){
                    printf("Unable to send message\n");
                    return -1;
                }
                fflush(stdin);
                printf("nhap username: ");
                scanf("%s", username);
                

                // Send the message to server:
                if(sendto(socket_desc, username, strlen(username), 0,
                    (struct sockaddr*)&server_addr, server_struct_length) < 0){
                    printf("Unable to send message\n");
                    return -1;
                }
                if(recvfrom(socket_desc, server_message, sizeof(server_message), 0,
                    (struct sockaddr*)&server_addr, &server_struct_length) < 0){
                    printf("Error while receiving server's msg\n");
                    return -1;
                }
                if(strcmp(server_message, "Insert password")==0){

                    //memset server_message
                    
                    printf("nhap password: ");
                    fflush(stdin);
                    scanf("%s",password);

                    if(sendto(socket_desc, password, strlen(password), 0,
                        (struct sockaddr*)&server_addr, server_struct_length) < 0){
                        printf("Unable to send message\n");
                        return -1;
                    }
                    
                    bzero(server_message, 1000);
                    if(recvfrom(socket_desc, server_message, sizeof(server_message), 0,
                        (struct sockaddr*)&server_addr, &server_struct_length) < 0){
                        printf("Error while receiving server's msg\n");
                        return -1;
                    }
                    // printf("%s\n",server_message);
                    while(!strcmp(server_message, "not OK")){
                        bzero(password, 100);
                        printf("%s\n",server_message);
                        printf("nhap  lai password: ");
                        fflush(stdin);
                        scanf("%s",password);
                        if(sendto(socket_desc, password, strlen(password), 0,
                            (struct sockaddr*)&server_addr, server_struct_length) < 0){
                            printf("Unable to send message\n");
                            return -1;
                        }
                        bzero(server_message, 1000);
                        if(recvfrom(socket_desc, server_message, sizeof(server_message), 0,
                            (struct sockaddr*)&server_addr, &server_struct_length) < 0){
                            printf("Error while receiving server's msg\n");
                            return -1;
                        }
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
                break;
            case 4:
                if(sendto(socket_desc, "bye", 3, 0,
                    (struct sockaddr*)&server_addr, server_struct_length) < 0){
                    printf("Unable to send message\n");
                    return -1;
                }
                // printf("abc\n");
                // Receive the server's response:
                bzero(server_message, 1000);
                if(recvfrom(socket_desc, server_message, sizeof(server_message), 0,
                    (struct sockaddr*)&server_addr, &server_struct_length) < 0){
                    printf("Error while receiving server's msg\n");
                    return -1;
                }
                printf("Goodbye: %s\n", server_message);
                // printMess(server_message);
                break;
            case 5:
                exit(1);
            case 6:
                if(sendto(socket_desc, "get data", sizeof("get data"), 0,
                    (struct sockaddr*)&server_addr, server_struct_length) < 0){
                    printf("Unable to send message\n");
                    return -1;
                }
                bzero(server_message, 1000);
                if(recvfrom(socket_desc, server_message, sizeof(server_message), 0,
                    (struct sockaddr*)&server_addr, &server_struct_length) < 0){
                    printf("Error while receiving server's msg\n");
                    return -1;
                }
                printMess(server_message);
                break;
            case 3:
                if(sendto(socket_desc, "send data", sizeof("send data"), 0,
                    (struct sockaddr*)&server_addr, server_struct_length) < 0){
                    printf("Unable to send message\n");
                    return -1;
                }
                bzero(client_message, 1000);
                printf("Enter message: ");
                fflush(stdin);
                scanf("%s", client_message);

                // Send the message to server:
                if(sendto(socket_desc, client_message, strlen(client_message), 0,
                    (struct sockaddr*)&server_addr, server_struct_length) < 0){
                    printf("Unable to send message\n");
                    return -1;
                }
                break;
            default:
                break;
        }
    }
    
    // Close the socket:
    close(socket_desc);
    
    return 0;
}

