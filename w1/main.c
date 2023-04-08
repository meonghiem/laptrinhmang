#include <stdio.h>
#include <string.h>
#include "dllist.h"
#include "fields.h"
#include <stdlib.h>

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

int main(){
    Dllist l = NULL;
    // printf("%p\n", l);
    // Dllist tmp = dll_next(dll_first(l));
    // Account acc = (Account)jval_v(dll_val(tmp));
    // printf("%s: %s: %d \n", acc->username, acc->password, acc->status);
    int choice, count=0, loggedIn =0;
    char username[100], password[100];
    while(1){
        printf("USER MANAGEMENT PROGRAM\n");
        printf("-----------------------------------\n");
        printf("1. Register\n");
        printf("2. Sign in\n");
        printf("3. Search\n");
        printf("4. Sign out\n");
        printf("5. Exit\n");
        printf("Your choice (1-4, other to quit):\n");
        scanf("%d",&choice);
        switch(choice){
            case 5: return 1; break;
            case 2:
                if(loggedIn){
                    printf("Logged In. So you must log out to log in\n");
                    break;
                }
                fflush(stdin);
                printf("nhap username: ");
                scanf("%s", username);
                printf("nhap password: ");
                fflush(stdin);
                scanf("%s",password);
                l = ListAccount("account.txt");
                if(checkAccountExist(l, username)){
                    Account acc = checkAccountExist(l,username);
                    if(checkPassWord(acc,password)){
                        printf("Hello %s\n", username);
                        loggedIn = 1;
                        count = 0;
                    }else{
                        count +=1;
                        while(count <3){
                            printf("password is incorrect => nhap lai password: ");
                            fflush(stdin);
                            scanf("%s",password);
                            if(checkPassWord(acc,password)){
                                printf("Hello %s\n", username);
                                loggedIn = 1;
                                count = 0;
                                break;
                            }else{
                                count ++;
                            }
                        }
                        if(count ==3){
                            printf("Password is incorrect. Account is blocked\n");
                            acc->status = 0;
                            count =0 ;
                            updateAccount("account.txt",l);
                        }
                    }
                }else{
                    printf("account does not exist\n");
                }
                free_dllist(l);
                break;
            case 3:
                if(!loggedIn){
                    printf("you are not logged in yet. So you must log in to use this function\n");
                    break;
                }else{
                    printf("type username to find: ");
                    fflush(stdin);
                    scanf("%s", username);
                    l = ListAccount("account.txt");
                    Account acc = checkAccountExist(l, username);
                    if(acc){
                        if(acc->status){
                            printf("Account is active\n");
                        }else{
                            printf("Account is blocked\n");
                        }
                    }else{
                        printf("cannot find account\n");
                    }
                    free_dllist(l);
                    break;
                }
            case 4:
                if(loggedIn){
                    loggedIn =0;
                    printf("Goodbye %s\n", username);
                }else{
                    printf("Account is not sign in\n");
                }
                break;
            case 1:
                if(loggedIn){
                    printf("you logged in . So you must log out to use this function\n");
                    break;
                }else{
                    fflush(stdin);
                    printf("nhap username: ");
                    scanf("%s", username);
                    printf("nhap password: ");
                    fflush(stdin);
                    scanf("%s",password);
                    l = ListAccount("account.txt");
                    if(!checkAccountExist(l,username)){
                        addAccount("account.txt", username, password);
                        printf("successful registration\n");
                    }else{
                        printf("account is existed\n");
                    }
                    free_dllist(l);
                    break;
                }
            default: printf("nhap lai\n");
            
        }

    }
}