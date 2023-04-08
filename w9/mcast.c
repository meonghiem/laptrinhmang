#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <pthread.h>
#include <sys/select.h>
#define NUMBER_CHARACTER 1024
#define NUMBER_NEIGHBOR 100

typedef struct _GroupList{
    struct sockaddr_in addr;
    int isMember;
    int time; //  to check thoi gian out
}GroupList;

typedef struct {
    int sendfd;
    int listenfd;
    struct sockaddr_in local_addr;
    struct sockaddr_in group_addr;
}Argument;

GroupList *groupList;

void *checkMember(void * index){
    int i = (*(int *)index);

    // check trong 30s de timeout
    groupList[i].time = 0 ;
    while(groupList[i].time <30){
        sleep(1);
        groupList[i].time++;
    }

    printf("Neigborgh %s %d was timeout\n", inet_ntoa(groupList[i].addr.sin_addr), groupList[i].addr.sin_port);
    bzero(&groupList[i].addr, sizeof(groupList[i].addr));
    groupList[i].isMember = -1;
    return NULL;
}

int isNewMember(struct sockaddr_in addr){
    int i=0;
    while(i < NUMBER_NEIGHBOR){
        if(groupList[i].addr.sin_addr.s_addr == addr.sin_addr.s_addr && groupList[i].addr.sin_port == addr.sin_port){
            return i;
        }
        i++;
    }
    return -1;
}

void *sendGroup(void * arg){
    pthread_detach(pthread_self());
    Argument * ARG = (Argument *) arg;
    int sendfd = ARG->sendfd;
    struct sockaddr_in group_addr = ARG->group_addr;
    char text[] = "hello";
    unsigned int length = sizeof(group_addr);

    // thuc hien viec gui toi group cu sau 10s
    do{
        sendto(sendfd, text, strlen(text), 0, (struct sockaddr *)&group_addr, sizeof(group_addr)); 
        sleep(10);
    }while(1);

    return NULL;
}



void *listenGroup(void * arg){
    pthread_detach(pthread_self());
    Argument *ARG = (Argument *) arg;
    int listenfd = ARG-> listenfd;
    struct sockaddr_in local_addr = ARG -> local_addr;
    char *buffer = calloc(NUMBER_CHARACTER, sizeof(char));
    pthread_t tid[NUMBER_NEIGHBOR];
    int buffer_size, i;
    unsigned int length = sizeof(local_addr);
    while(1){
        bzero(buffer, NUMBER_CHARACTER);
        buffer_size = recvfrom(listenfd, buffer, NUMBER_CHARACTER, 0, (struct sockaddr *)&local_addr, &length);
        buffer[buffer_size] = '\0';
        if(buffer_size > 0){

            i = isNewMember(local_addr);
            if(i < 0){
                i=0;
                while(i < NUMBER_NEIGHBOR){
                    if(groupList[i].isMember < 0){
                        groupList[i].isMember = 1;
                        groupList[i].addr = local_addr;
                        pthread_create(&tid[i], NULL, checkMember, (void *)&i);
                        printf("New Node: %s %d\n", inet_ntoa(groupList[i].addr.sin_addr), groupList[i].addr.sin_port);
                        break;
                    }
                    i++;
                }
            }else{
                groupList[i].time =0;
            }
        }else{
            perror("Recvfrom failed\n");
            exit(0);
        }
    }
    return NULL;
}

int main(int argc, char const *argv[]){
    
    int sendfd, listenfd;

    fd_set readfd;

    struct sockaddr_in local_addr, group_addr;
    struct ip_mreq group;
    struct in_addr localInterface;

    pthread_t tid_send;
    pthread_t tid_recv;
    int i;
    groupList = calloc(NUMBER_NEIGHBOR, sizeof(GroupList));
    for(i = 0; i < NUMBER_NEIGHBOR; i++){
        bzero(&groupList[i].addr, sizeof(groupList[i].addr));
        groupList[i].isMember = -1;
    }
    if(argc < 3 ){
        printf("Server creation failed! Need port number! Usage: %s [port_number]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[2]);

    // setupSocket(port, argv[1]);
    const char * group_ip = argv[1];

    // setup server
    if((sendfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1 || (listenfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }   

    bzero(&group_addr, sizeof(group_addr));
    group_addr.sin_family = AF_INET;
    group_addr.sin_port = htons(port);
    group_addr.sin_addr.s_addr = inet_addr(group_ip);

    bzero(&local_addr, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port);
    local_addr.sin_addr.s_addr = INADDR_ANY;


    char loop = 0;
    if(setsockopt(sendfd, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loop, sizeof(loop)) < 0){
        perror("Set option socket group failed");
        exit(EXIT_FAILURE);
    }

    localInterface.s_addr = inet_addr("127.0.0.1");

    if(setsockopt(sendfd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0){
        perror("Set local interface failed");
        exit(EXIT_FAILURE);
    }

    int reuse = 1;
    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0){
        perror("Set option socket reuse failed");
        exit(EXIT_FAILURE);
    }
    if(bind(listenfd, (struct sockaddr *)&local_addr, sizeof(local_addr)) != 0){
        perror("Socket listen bind failed");
        exit(EXIT_FAILURE);
    }
    group.imr_multiaddr.s_addr = inet_addr(group_ip);
    group.imr_interface.s_addr = inet_addr("127.0.0.1");
    if(setsockopt(listenfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0){
        perror("Set group multicast failed");
        exit(EXIT_FAILURE);
    }

    // =============
    Argument * arg = calloc(1, sizeof(Argument));
    arg->listenfd = listenfd;
    arg->sendfd = sendfd;
    arg->local_addr = local_addr;
    arg->group_addr = group_addr;

    pthread_create(&tid_send, NULL, sendGroup, (void *)arg);

    pthread_create(&tid_recv, NULL, listenGroup, (void *) arg);

    FD_ZERO(&readfd);
    FD_SET(STDIN_FILENO, &readfd);
    int max_fd = STDIN_FILENO;
    int nEvent;
    while(1){
        nEvent = select(max_fd + 1, &readfd, NULL ,NULL, NULL);
        if(nEvent < 0){
            perror("Select error");
            exit(EXIT_FAILURE);
        }
        if(FD_ISSET(STDIN_FILENO, &readfd)){
            // command_handle();
            char *text = calloc(NUMBER_CHARACTER, sizeof(char));

            int text_size = read(STDIN_FILENO, text, NUMBER_CHARACTER);
            text[text_size - 1] = '\0';
            if(strcmp(text, "quit") == 0){
                close(sendfd);
                close(listenfd);
                exit(EXIT_SUCCESS);
            }else if(strcmp(text, "print mtable") == 0){
                // printMember();
                int i;
                printf("List of neighbors:\n");
                for(i = 0; i< NUMBER_NEIGHBOR; i++){
                    if(groupList[i].isMember > 0){
                        printf("%s %d\n", inet_ntoa(groupList[i].addr.sin_addr), groupList[i].addr.sin_port);
                    }
                }
            }else{
                printf("sai command\n");
            }
        }
    }
    

    close(listenfd);
    return 0;
}