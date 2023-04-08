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
#include "dllist.h"
#include "fields.h"
#include "jval.h"

int check(char *message){
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

typedef struct Account{
    char *username;
    char *password;
    int status;
}*Account;

// create a new account
Account newAccount(char *username, char *password, int status){
    Account tmp = (Account) malloc(sizeof(struct Account));
    tmp->username = strdup(username);
    tmp->password = strdup(password);
    tmp->status = status;
   
    return tmp;
}

// read from account.txt and then save them to Dllist
Dllist ListAccount(char * filename){
    IS is;
    Dllist l = new_dllist();
    is = new_inputstruct(filename);
    if(is == NULL){
        printf("not file name\n");
        exit(0);
    }
    while(get_line(is) >= 0) {
        Account newAcc = newAccount(is->fields[0], is->fields[1], atoi(is->fields[2]));
        dll_append(l,new_jval_v((void *) newAcc));
    }
    jettison_inputstruct(is);
    return l;
}

// check whether username is exist?
Account checkAccountExist(Dllist l, char *username){
    Dllist tmp;
    dll_traverse(tmp, l){
        Account acc = (Account)jval_v(dll_val(tmp));
        if(strcmp(username, acc->username) == 0) return acc;
    }
    return NULL;
}

// check whether password is correct?
int checkPassWord(Account acc, char*password){
    if(strcmp(password, acc->password) == 0) return 1;
    return 0;
}

// add a Account to file account.txt
void addAccount(char *filename, char*username, char *password){
    FILE *fp;
    fp = fopen(filename, "a");
    fprintf(fp,"%s %s %d\n", username, password, 1);
    fclose(fp);
}

void updateAccount(char *filename, Dllist l){
    FILE *fp;
    fp = fopen(filename, "w+");
    Dllist tmp;
    // Account acc;
    if(fp !=NULL){
        dll_traverse(tmp,l){
            Account acc = (Account)jval_v(dll_val(tmp));
            fprintf(fp,"%s %s %d\n", acc->username, acc->password, acc->status);
        }
    }
    fclose(fp);
}

int main(int argc, char * argv[]){
    if(argc !=2){
        printf("nhap port\n");
        return 0;
    }
    int socket_desc;
    struct sockaddr_in server_addr, client_addr;
    char server_message[2000], client_message[2000];
    char message[1000];
    char output[1000];
    int isLogged = 0;
    int count =0;
    int isUsername = 1; // stand by client_message is username?
    char usernameLogged[100], username[100], password[100];

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
        
        if(!strcmp(client_message, "Sign in")){
            Dllist l = NULL;
            bzero(client_message, 1000);
            if (recvfrom(socket_desc, client_message, sizeof(client_message), 0,
                (struct sockaddr*)&client_addr, &client_struct_length) < 0){
                printf("Couldn't receive\n");
                return -1;
            }
            // recvfrom(socket_desc, client_message, sizeof(client_message), 0,
            //     (struct sockaddr*)&client_addr, &client_struct_length);
            printf("Received message from IP: %s and port: %i\n",
                inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            

            strcpy(username, client_message);
            // isUsername = 0;

            bzero(client_message,1000);

            if (sendto(socket_desc, "Insert password", strlen("Insert password"), 0,
                (struct sockaddr*)&client_addr, client_struct_length) < 0){
                printf("Can't send\n");
                return -1;
            }

            if (recvfrom(socket_desc, client_message, sizeof(client_message), 0,
                (struct sockaddr*)&client_addr, &client_struct_length) < 0){
                printf("Couldn't receive\n");
                return -1;
            }

            strcpy(password, client_message);
            bzero(client_message, 1000);

            l = ListAccount("account.txt");
            if(isLogged){
                if (sendto(socket_desc, "Account not ready", strlen("Account not ready"), 0,
                    (struct sockaddr*)&client_addr, client_struct_length) < 0){
                    printf("Can't send\n");
                    return -1;
                }
                bzero(username, 100);
                bzero(password, 100);
            }else{
                if(checkAccountExist(l,username)){
                    Account acc = checkAccountExist(l, username);
                    if(!acc->status){
                        if (sendto(socket_desc, "Account is blocked", strlen("Account is blocked"), 0,
                            (struct sockaddr*)&client_addr, client_struct_length) < 0){
                            printf("Can't send\n");
                            return -1;
                        }
                    }else{
                        if(checkPassWord(acc, password)){
                            if (sendto(socket_desc, "OK", strlen("OK"), 0,
                                (struct sockaddr*)&client_addr, client_struct_length) < 0){
                                printf("Can't send\n");
                                return -1;
                            }
                            strcpy(usernameLogged, username);
                            isLogged = 1;
                            bzero(username, 100);
                            bzero(password, 100);
                        }else{
                            count ++;
                            while(count < 3){
                                if (sendto(socket_desc, "not OK", strlen("not OK"), 0,
                                    (struct sockaddr*)&client_addr, client_struct_length) < 0){
                                    printf("Can't send\n");
                                    return -1;
                                }
                                bzero(client_message, 1000);
                                if (recvfrom(socket_desc, client_message, sizeof(client_message), 0,
                                    (struct sockaddr*)&client_addr, &client_struct_length) < 0){
                                    printf("Couldn't receive\n");
                                    return -1;
                                }
                                bzero(password, 100);
                                strcpy(password, client_message);
                                if(checkPassWord(acc,password)){
                                    if (sendto(socket_desc, "OK", strlen("OK"), 0,
                                        (struct sockaddr*)&client_addr, client_struct_length) < 0){
                                        printf("Can't send\n");
                                        return -1;
                                    }
                                    strcpy(usernameLogged, username);
                                    isLogged = 1;
                                    bzero(username, 100);
                                    bzero(password, 100);
                                }else{
                                    count ++;
                                }
                            }
                            if(count ==3){
                                if (sendto(socket_desc, "Account is blocked", strlen("Account is blocked"), 0,
                                    (struct sockaddr*)&client_addr, client_struct_length) < 0){
                                    printf("Can't send\n");
                                    return -1;
                                }
                                acc->status =0;
                                count =0;
                                updateAccount("account.txt", l);
                            }
                        }
                    }
                }
            }
        }else if(!strcmp(client_message,"bye")){
            if(!strcmp(client_message, "bye")){
                if(isLogged){
                    isLogged =0;
                    if(sendto(socket_desc, usernameLogged, sizeof(usernameLogged), 0,
                        (struct sockaddr*)&client_addr, client_struct_length) < 0){
                        printf("Unable to send message\n");
                        return -1;
                    }
                } 
            } 
        }else if(!strcmp(client_message,"get data")){
            if(check(message)){
                // printf("\ngui di%s", message);
                if (sendto(socket_desc, message, strlen(message), 0,
                    (struct sockaddr*)&client_addr, client_struct_length) < 0){
                    printf("Can't send\n");
                    return -1;
                }
                bzero(message, 1000);
            }else{
                if (sendto(socket_desc, "error", strlen("error"), 0,
                    (struct sockaddr*)&client_addr, client_struct_length) < 0){
                    printf("Can't send\n");
                    return -1;
                }
            }
        }else if(!strcmp(client_message, "send data")){
            bzero(client_message, 1000);
            if (recvfrom(socket_desc, client_message, sizeof(client_message), 0,
                (struct sockaddr*)&client_addr, &client_struct_length) < 0){
                printf("Couldn't receive\n");
                return -1;
            }
            strcpy(message,client_message);
        }

        






        // printf("Msg from client: %s\n", client_message);
        // if(strcmp(client_message, "***")==0){
        //     // Respond to client:
        //     if(check(server_message)){
        //         // printf("\ngui di%s", server_message);
        //         if (sendto(socket_desc, server_message, strlen(server_message), 0,
        //         (struct sockaddr*)&client_addr, client_struct_length) < 0){
        //         printf("Can't send\n");
        //         return -1;
        //         }
        //     }else{
        //         if (sendto(socket_desc, "error", strlen("error"), 0,
        //         (struct sockaddr*)&client_addr, client_struct_length) < 0){
        //         printf("Can't send\n");
        //         return -1;
        //         }
        //     }            
            
        //     memset(server_message, '\0', sizeof(server_message));


        // }else{
        //     strcpy(server_message, client_message);
        //     memset(client_message, '\0', sizeof(client_message));
            
        //     printf("getdata: %s\n", server_message);
        // }
    }
    
    // Close the socket:
    close(socket_desc);
    
    return 0;
}
