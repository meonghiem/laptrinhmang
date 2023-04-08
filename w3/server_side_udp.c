#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int check(char *message){
	// char digit[1000];
    // char alpha[1000];
    // bzero(digit,1000);
    // bzero(alpha, 1000);
    // memset(digit, '\0', sizeof(digit));
    // memset(alpha, '\0', sizeof(alpha));
	int i;
	for(i = 0; i < strlen(message); i++){
		if(message[i] <= '9' && message[i] >= '0'){
            continue;
		}else if((message[i] <= 'z' && message[i] >= 'a')){
			continue;
		}else if((message[i] <= 'Z' && message[i] >= 'A')){
			continue;
		}else
		{
			return 0;
		}
		
	}
	return 1;
}
int main(int argc, char * argv[]){
    if(argc !=2){
        printf("nhap port\n");
        return 0;
    }
    int socket_desc;
    struct sockaddr_in server_addr, client_addr;
    char server_message[2000], client_message[2000];
    char output[1000];

    int client_struct_length = sizeof(client_addr);
    
    // Clean buffers:
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));
    memset(output, '\0', sizeof(output));

    // Create UDP socket:
    socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    if(socket_desc < 0){
        printf("Error while creating socket\n");
        return -1;
    }
    printf("Socket created successfully\n");
    
    // Set port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    // Bind to the set port and IP:
    if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        printf("Couldn't bind to the port\n");
        return -1;
    }
    printf("Done with binding\n");
    
    printf("Listening for incoming messages...\n\n");
    
    while(1){
        // Receive client's message:
        bzero(client_message, 1000);
        if (recvfrom(socket_desc, client_message, sizeof(client_message), 0,
            (struct sockaddr*)&client_addr, &client_struct_length) < 0){
            printf("Couldn't receive\n");
            return -1;
        }
        printf("Received message from IP: %s and port: %i\n",
            inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        

        printf("Msg from client: %s\n", client_message);
        if(strcmp(client_message, "***")==0){
            // Respond to client:
            if(check(server_message)){
                // printf("\ngui di%s", server_message);
                if (sendto(socket_desc, server_message, strlen(server_message), 0,
                (struct sockaddr*)&client_addr, client_struct_length) < 0){
                printf("Can't send\n");
                return -1;
                }
            }else{
                if (sendto(socket_desc, "error", strlen("error"), 0,
                (struct sockaddr*)&client_addr, client_struct_length) < 0){
                printf("Can't send\n");
                return -1;
                }
            }            
            
            memset(server_message, '\0', sizeof(server_message));


        }else{
            strcpy(server_message, client_message);
            memset(client_message, '\0', sizeof(client_message));
            
            printf("getdata: %s\n", server_message);
        }
    }
    
    // Close the socket:
    close(socket_desc);
    
    return 0;
}
