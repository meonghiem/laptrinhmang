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
#include <pthread.h>

#define PORT 5550
#define BACKLOG 20
#define BUFF_SIZE 1024

typedef struct argument{
    int sockfd;
    char * username;
} argument;
/* Handler process signal*/
void sig_chld(int signo);

/*
* Receive and echo message to client
* [IN] sockfd: socket descriptor that connects to client 	
*/
void* echo(void * arg);

int main(int argc, char * argv[]){
	if(argc !=2){
        printf("nhap port\n");
        return 0;
    }
	int listen_sock, conn_sock; /* file descriptors */
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	pthread_t tid;
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
	// signal(SIGCHLD, sig_chld);

	while(1){
        loop:
		sin_size=sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock, (struct sockaddr *)&client, &sin_size))==-1){
			// if (errno == EINTR)
			// 	continue;
			// else{
			// }
				perror("\nError: ");			
				return 0;
		}
		
		/* For each client, fork spawns a child, and the child handles the new client */
		// pid = fork();
		// /* fork() is called in child process */
		// if(pid  == 0){
		// 	close(listen_sock);
		// 	// printf("You got a connection from %s\n", inet_ntoa(client.sin_addr)); /* prints client's IP */
		// 	printf("connected\n");
		// 	echo(conn_sock);
		// 	exit(0);
		// }
        int * arg = &conn_sock;
        printf("abc\n");
        pthread_create(&tid,NULL, &echo,(int *)arg);
		
		/* The parent closes the connected socket since the child handles the new client */
		// close(conn_sock);
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

//read whole file
void readFile(char *file, char *buff){
    FILE* ptr;
    char ch;
    int i=0;
 
    // Opening file in reading mode
    ptr = fopen(file, "r");
 
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

void sendMessage(char * file, int sockfd){
    char buff[100];
    char message[1000000];
	int bytes_sent, bytes_received;
    bzero(message,1000000);
    readFile(file,message);
    printf("message: %s\n", message);
        bytes_received = recv(sockfd, buff, 100, 0);
		if (bytes_received < 0)perror("\nError: ");
        printf("%s\n", buff);

        bytes_sent = send(sockfd, message, strlen(message), 0); /* echo to the client */
		if (bytes_sent < 0)perror("\nError: ");
    return;
}

void otherSendMessage(char *buff, int sockfd){
    int bytes_received, bytes_sent;
    bytes_sent = send(sockfd, buff, strlen(buff), 0);
	if(bytes_sent < 0){
		perror("\nError \n");
	}
	bytes_received = recv(sockfd, buff, BUFF_SIZE-1, 0);
		if(bytes_received < 0){
			perror("\nError!Cannot receive data from sever!\n");
		}
}

void writeFileLater(char *file, char *username, char* message){
    FILE * fp = fopen(file, "a");
    if(!fp){
        perror("not open file\n");
        exit(0);
    }
    // printf("%s: %s\n", username, message);
    fprintf(fp,"%s : %s\n", username, message);
    fclose(fp);
    return;
}

void getUsername(char *buff, char *username, char * message){
    int k=0,j=0;
	int flag=0;
	for(int i=0;i<strlen(buff);i++){
		if(buff[i]==' '&&flag==0){
			flag=1;
		}
		if(flag == 0) username[k++]=buff[i];
		if(flag == 1) message[j++]= buff[i];
	}
	username[k]='\0';
	message[j]='\0';
}

void * handlerrr(void * arg){
    argument * ARG = (argument *)arg;
    char * username = ARG->username;
    int sockfd = ARG->sockfd;
    int bytes_sent, bytes_received;
    char buff[100];
    char userName[100], MESSAGE[10000];
    pthread_detach(pthread_self());
    FILE * f = fopen("message.txt", "a");
        int oldsize = ftell(f);
        // printf("%d\n",oldsize);
        fclose(f);
        while(1){
            sleep(1);
            FILE * p = fopen("message.txt", "a");
            int newsize = ftell(p);
            fclose(p);
            p = fopen("message.txt", "r");
            // printf("newsize: %d", newsize);
            if(newsize > oldsize){
                fseek(p, oldsize, SEEK_SET);
                fgets(buff, 100, p);
                printf("buff: %s\n", buff);
                bzero(userName,100);
                bzero(MESSAGE,10000);
                printf("nhay vao day\n");
                getUsername(buff,userName, MESSAGE);
                if(strcmp(username,userName)!= 0 ){
                    // otherSendMessage(buff,sockfd);
                    printf("nhay vao day\n");

                    bytes_sent = send(sockfd, buff, strlen(buff), 0);
                    if(bytes_sent < 0){
                        perror("\nError \n");
                    }
                }

                printf("298 %s\n", MESSAGE);
                if(strcmp(username, userName) == 0 && strcmp(MESSAGE," left room\n") ==0){
                    // otherSendMessage(buff, sockfd);
                    printf("sockfd %d\n", sockfd);
                    printf("buff %s\n", buff);
                    bytes_sent = send(sockfd, buff, strlen(buff), 0);
                    if(bytes_sent < 0){
                        perror("\nError bac \n");
                    }
                    break;
                }
                oldsize = newsize;
            }
            fclose(p);
        }
        printf("account %s log out\n", userName);
        sleep(1);
        close(sockfd);

}

void getMessageFromClient(int sockfd, char *username){
	int bytes_sent, bytes_received;
    char buff[100];
    char userName[100], MESSAGE[10000];
    // bzero(buff, 100);
    // bytes_received = recv(sockfd, buff, BUFF_SIZE-1, 0);
	// if(bytes_received < 0){
	// 	perror("\nError!Cannot receive data from sever!\n");
	// }
    // signal(SIGCHLD,sig_chld);
    // pid_t pid =fork();

    pthread_t tid;
    argument * arg = calloc(1,sizeof(argument));
    arg->sockfd = sockfd;
    arg->username = calloc(100, sizeof(char));
    strcpy(arg->username, username);
    pthread_create(&tid, NULL, &handlerrr, (argument *)arg);
    // if(pid == 0){
    //     FILE * f = fopen("message.txt", "a");
    //     int oldsize = ftell(f);
    //     // printf("%d\n",oldsize);
    //     fclose(f);
    //     while(1){
    //         sleep(1);
    //         FILE * p = fopen("message.txt", "a");
    //         int newsize = ftell(p);
    //         fclose(p);
    //         p = fopen("message.txt", "r");
    //         // printf("newsize: %d", newsize);
    //         if(newsize > oldsize){
    //             fseek(p, oldsize, SEEK_SET);
    //             fgets(buff, 100, p);
    //             printf("buff: %s\n", buff);
    //             bzero(userName,100);
    //             bzero(MESSAGE,10000);
    //             printf("nhay vao day\n");
    //             getUsername(buff,userName, MESSAGE);
    //             if(strcmp(username,userName)!= 0 ){
    //                 // otherSendMessage(buff,sockfd);
    //                 printf("nhay vao day\n");

    //                 bytes_sent = send(sockfd, buff, strlen(buff), 0);
    //                 if(bytes_sent < 0){
    //                     perror("\nError \n");
    //                 }
    //             }

    //             printf("298 %s\n", MESSAGE);
    //             if(strcmp(username, userName) == 0 && strcmp(MESSAGE," left room\n") ==0){
    //                 // otherSendMessage(buff, sockfd);
    //                 printf("1234\n");
    //                 bytes_sent = send(sockfd, buff, strlen(buff), 0);
    //                 if(bytes_sent < 0){
    //                     perror("\nError \n");
    //                 }
    //                 break;
    //             }
    //             oldsize = newsize;
    //         }
    //         fclose(p);
    //     }
    //     printf("account %s log out\n", userName);
    //     close(sockfd);
    //     exit(0);
    // }

    while(1){

        bzero(buff,100);
        bytes_received = recv(sockfd, buff, 100, 0);
		if (bytes_received < 0)perror("\nError: ");
        // bytes_sent = send(sockfd, "got message", strlen("got message"), 0); /* echo to the client */
		// if (bytes_sent < 0)perror("\nError: ");
        // buff[strlen(buff)-1] ='\0';
        // printf("%s: %s\n",username,buff);
        if(strcmp(buff,"/exit")==0){
            printf("exit\n");
            FILE * fpp = fopen("message.txt", "a+");
            // char mes[1000];
            // bzero(mes,1000);
            // sprintf(mes, "%s left room\n", userName);
            // printf("%s \n", mes);
            printf("%s\n", username);
            fprintf(fpp, "%s left room\n",username);
            // fputs(fpp, mes);
            // close(sockfd);
            fclose(fpp);
            return;
        }else{

            // printf("dang ghi vao file\n");
            writeFileLater("message.txt", username, buff);
        }
    }
}


void* echo(void *arg) {
    pthread_detach(pthread_self());
    int sockfd = *((int*)arg);
    printf("sockfd %d\n", sockfd);
	// char buff[BUFF_SIZE];
	int bytes_sent, bytes_received;
	int isLogged =0;
	char usernameLogged[100], username[100], password[100];
	bzero(username,100);
	bzero(password,100);
	bzero(usernameLogged,100);
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

        printf("asdjlfsf\n");
		bzero(password,100);
		bytes_received = recv(sockfd, password, 100, 0);
		if (bytes_received < 0)perror("\nError: ");

		l= ListAccount("account.txt");
		if(checkAccountExist(l,username)){
                    Account acc = checkAccountExist(l, username);
                    if(!acc->status){
                        bytes_sent = send(sockfd, "Account is blocked", strlen("Account is blocked"), 0); /* echo to the client */
						if (bytes_sent < 0)perror("\nError: ");
                        printf("211: sign in failure\n");
                        exit(0);
                    }else{
                        if(checkPassWord(acc, password)){
                            bytes_sent = send(sockfd, "OK", strlen("OK"), 0); /* echo to the client */
							if (bytes_sent < 0)perror("\nError: ");
                            bzero(usernameLogged,100);
                            strcpy(usernameLogged, username);
                            isLogged = 1;
                            bzero(username, 100);
                            bzero(password, 100);
                        }else{
                            bytes_sent = send(sockfd, "not OK", strlen("not OK"), 0); /* echo to the client */
								if (bytes_sent < 0)perror("\nError: ");
                                exit(0);
                            // count ++;
                            // while(count < 3){
                            //     bytes_sent = send(sockfd, "not OK", strlen("not OK"), 0); /* echo to the client */
							// 	if (bytes_sent < 0)perror("\nError: ");
                            //     bzero(password,100);
							// 	bytes_received = recv(sockfd, password, 100, 0);
							// 	if (bytes_received < 0)perror("\nError: ");
                            //     if(checkPassWord(acc,password)){
                            //         bytes_sent = send(sockfd, "OK", strlen("OK"), 0); /* echo to the client */
							// 		if (bytes_sent < 0)perror("\nError: ");
                            //         bzero(usernameLogged,100);
                            //         strcpy(usernameLogged, username);
                            //         isLogged = 1;
                            //         bzero(username, 100);
                            //         bzero(password, 100);
							// 		count =0;
							// 		break;
                            //     }else{
                            //         count ++;
                            //     }
                            // }
                            // if(count ==3){
                            //     bytes_sent = send(sockfd, "Account is blocked", strlen("Account is blocked"), 0); /* echo to the client */
							// 	if (bytes_sent < 0)perror("\nError: ");
                            //     acc->status =0;
                            //     count =0;
                            //     updateAccount("account.txt", l);
                            //     printf("248:sign in failure");
                            //     exit(0);
                            // }
                        }
                    }

            //show message after joining room
        printf("show message");
        sendMessage("message.txt", sockfd);
        //end show message

        //get new message from client
        printf("%d\n", sockfd);
        getMessageFromClient(sockfd,usernameLogged);
        break;
		}else{
			bytes_sent = send(sockfd, "username error", strlen("username error"), 0); /* echo to the client */
			if (bytes_sent < 0)perror("\nError: ");
            printf("257: sign in failure\n");
            exit(0);
		}

        // //show message after joining room
        // printf("show message");
        // sendMessage("message.txt", sockfd);
        // //end show message

        // //get new message from client
        // getMessageFromClient(sockfd,usernameLogged);


		// bytes_sent = send(sockfd, "OK", sizeof("OK"), 0); /* echo to the client */
		// if (bytes_sent < 0)perror("\nError: ");

	}
			
	// close(sockfd);

}
