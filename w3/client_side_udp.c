#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

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
    int server_struct_length = sizeof(server_addr);
    
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
    while(1){
        printf("1. Input data\n");
        printf("2. Get Data\n");
        memset(server_message, '\0', sizeof(server_message));
        memset(client_message, '\0', sizeof(client_message));
        printf("nhap choice: ");
        scanf("%d", &choice);
        switch(choice){
            case 1:
                // Get input from the user:
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
            case 2:
                if(sendto(socket_desc, "***", 3, 0,
                    (struct sockaddr*)&server_addr, server_struct_length) < 0){
                    printf("Unable to send message\n");
                    return -1;
                }
                // printf("abc\n");
                // Receive the server's response:
                // bzero(server_message, 1000);
                if(recvfrom(socket_desc, server_message, sizeof(server_message), 0,
                    (struct sockaddr*)&server_addr, &server_struct_length) < 0){
                    printf("Error while receiving server's msg\n");
                    return -1;
                }
                
                // printf("Server's response: %s\n", server_message);
                printMess(server_message);
                break;
            default:
                break;
        }
    }
    
    // Close the socket:
    close(socket_desc);
    
    return 0;
}

