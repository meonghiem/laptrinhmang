#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5550
#define BUFF_SIZE 1024

// int check_period(char *string)
// {
//     int count_period = 0, n = strlen(string);

//     if (string[0] == '.')
//         return 0;
//     if (string[n - 1] == '.')
//         return 0;
//     for (int i = 0; i < n - 1; i++)
//     {
//         if (string[i] == '.')
//             count_period++;
//         if (string[i] == '.' && string[i + 1] == '.')
//             return 0;
//     }
//     if (count_period != 3)
//         return 0;
//     return 1;
// }

// int check_IP(char *string)
// {
//     int value = 0, n = strlen(string);
//     if (check_period(string) == 0)
//     {
//         return 0;
//     }
//     else
//     {
//         for (int i = 0; i < n; i++)
//         {
//             if (string[i] == '.')
//             {
//                 if (value < 0 || value > 255)
//                     return 0;
//                 value = 0;
//             }
//             else
//             {
//                 if (string[i] >= '0' && string[i] <= '9')
//                 {
//                     value = value * 10 + (string[i] - '0');
//                     if (i == n - 1)
//                         if (value < 0 || value > 255)
//                             return 0;
//                 }
//                 else
//                     return 0;
//             }
//         }
//         return 1;
//     }
// }

int main(int argc, char *argv[]){
	if (argc != 3)
    {
        printf("Please input portnumber and ip address!\n");
        exit(1);
    }
    // if (check_IP(argv[1]) == 0)
    // {
    //     printf("IP address invalid\n");
    //     return 0;
    // }
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
		printf("\nSend massage to server! ");

		
	//Step 4: Communicate with server			
	
	//send message	
	sprintf(buff, "server address: %s,port: %d, socket port: %d\n",  SERVER_ADDR, SERVER_PORT, client_sock);
	// msg_len = strlen(buff);

	// printf("%s", buff);		
	bytes_sent = send(client_sock, buff, strlen(buff), 0);
	if(bytes_sent < 0)
		perror("\nError: ");
	
	//receive echo reply
	// bytes_received = recv(client_sock, buff, BUFF_SIZE, 0);
	// if (bytes_received < 0)
	// 		perror("\nError: ");
	// else if (bytes_received == 0)
	// 		printf("Connection closed.\n");
		
	// buff[bytes_received] = '\0';
	// printf("Reply from server: %s", buff);
	
	
	//Step 4: Close socket
	close(client_sock);
	return 0;
}
