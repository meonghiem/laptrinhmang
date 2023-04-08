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

#include <string.h>
#include <ctype.h>
#include "dllist.h"
#include "fields.h"
#include "jval.h"

#define PORT 5550
#define BACKLOG 20
#define BUFF_SIZE 1024

/* Handler process signal*/
void sig_chld(int signo);

/*
* Receive and echo message to client
* [IN] sockfd: socket descriptor that connects to client 	
*/
void echo(int sockfd);

int main(int argc, char * argv[]){
	if(argc !=2){
        printf("nhap port\n");
        return 0;
    }
	int listen_sock, conn_sock; /* file descriptors */
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	pid_t pid;
	unsigned int sin_size;

	if ((listen_sock=socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		printf("socket() error\n");
		return 0;
	}
	
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;         
	server.sin_port = htons(atoi(argv[1]));
	server.sin_addr.s_addr = htonl(INADDR_ANY);  /* INADDR_ANY puts your IP address automatically */   

	if(bind(listen_sock, (struct sockaddr*)&server, sizeof(server))==-1){ 
		perror("\nError: ");
		return 0;
	}     

	if(listen(listen_sock, BACKLOG) == -1){  
		perror("\nError: ");
		return 0;
	}
	
	/* Establish a signal handler to catch SIGCHLD */
	signal(SIGCHLD, sig_chld);

	while(1){
		sin_size=sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock, (struct sockaddr *)&client, &sin_size))==-1){
			if (errno == EINTR)
				continue;
			else{
				perror("\nError: ");			
				return 0;
			}
		}
		
		/* For each client, fork spawns a child, and the child handles the new client */
		pid = fork();
		/* fork() is called in child process */
		if(pid  == 0){
			close(listen_sock);
			// printf("You got a connection from %s\n", inet_ntoa(client.sin_addr)); /* prints client's IP */
			printf("connected\n");
			echo(conn_sock);
			exit(0);
		}
		
		/* The parent closes the connected socket since the child handles the new client */
		close(conn_sock);
	}
	close(listen_sock);
	return 0;
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

void sig_chld(int signo){
	pid_t pid;
	int stat;
	
	/* Wait the child process terminate */
	while((pid = waitpid(-1, &stat, WNOHANG))>0)
		printf("\nChild %d terminated\n",pid);
}

void echo(int sockfd) {
	// char buff[BUFF_SIZE];
	int bytes_sent, bytes_received;
	int isLogged =0;
	char usernameLogged[100], username[100], password[100];
	bzero(username,0);
	bzero(password,0);
	bzero(usernameLogged,0);
	Dllist l;
	int count=0;

	printf("cbi recvieve\n");
	// bytes_received = recv(sockfd, buff, BUFF_SIZE, 0); //blocking
	// if (bytes_received < 0)
	// 	perror("\nError: ");
	// else if (bytes_received == 0)
	// 	printf("Connection closed.");
	
	// bytes_sent = send(sockfd, buff, bytes_received, 0); /* echo to the client */
	// if (bytes_sent < 0)
	// 	perror("\nError: ");

	while(!isLogged){
		bzero(username,100);
		bytes_received = recv(sockfd, username, 100, 0);
		if (bytes_received < 0)perror("\nError: ");
		bytes_sent = send(sockfd, "Insert password", strlen("Insert password"), 0); /* echo to the client */
		if (bytes_sent < 0)perror("\nError: ");
		bzero(password,100);
		bytes_received = recv(sockfd, password, 100, 0);
		if (bytes_received < 0)perror("\nError: ");

		l= ListAccount("account.txt");
		if(checkAccountExist(l,username)){
                    Account acc = checkAccountExist(l, username);
                    if(!acc->status){
                        bytes_sent = send(sockfd, "Account is blocked", strlen("Account is blocked"), 0); /* echo to the client */
						if (bytes_sent < 0)perror("\nError: ");
                    }else{
                        if(checkPassWord(acc, password)){
                            bytes_sent = send(sockfd, "OK", strlen("OK"), 0); /* echo to the client */
							if (bytes_sent < 0)perror("\nError: ");
                            strcpy(usernameLogged, username);
                            isLogged = 1;
                            bzero(username, 100);
                            bzero(password, 100);
                        }else{
                            count ++;
                            while(count < 3){
                                bytes_sent = send(sockfd, "not OK", strlen("not OK"), 0); /* echo to the client */
								if (bytes_sent < 0)perror("\nError: ");
                                bzero(password,100);
								bytes_received = recv(sockfd, password, 100, 0);
								if (bytes_received < 0)perror("\nError: ");
                                if(checkPassWord(acc,password)){
                                    bytes_sent = send(sockfd, "OK", strlen("OK"), 0); /* echo to the client */
									if (bytes_sent < 0)perror("\nError: ");
                                    strcpy(usernameLogged, username);
                                    isLogged = 1;
                                    bzero(username, 100);
                                    bzero(password, 100);
									count =0;
									break;
                                }else{
                                    count ++;
                                }
                            }
                            if(count ==3){
                                bytes_sent = send(sockfd, "Account is blocked", strlen("Account is blocked"), 0); /* echo to the client */
								if (bytes_sent < 0)perror("\nError: ");
                                acc->status =0;
                                count =0;
                                updateAccount("account.txt", l);
                            }
                        }
                    }
		}else{
			bytes_sent = send(sockfd, "username error", strlen("username error"), 0); /* echo to the client */
			if (bytes_sent < 0)perror("\nError: ");
		}

		// bytes_sent = send(sockfd, "OK", sizeof("OK"), 0); /* echo to the client */
		// if (bytes_sent < 0)perror("\nError: ");

	}
			
	close(sockfd);
}