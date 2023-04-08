#include <string.h>
#include <ctype.h>
#include "dllist.h"
#include "fields.h"
#include "jval.h"
#include <stdio.h>
#include <stdlib.h>

int main(){
    // IS is;
    // is = new_inputstruct("message.txt");
    // if (is == NULL) {
    //     perror("message.txt");
    //     exit(1);
    // }
    // int count =0;
    // while(get_line(is) >=0){
    //     printf("%d: %s\n", count++, is->text1);
    // }
    // jettison_inputstruct(is);
    // return 0;
    FILE * fp = fopen("message.txt", "a");
    if(!fp){
        perror("not open file\n");
        exit(0);
    }
    char username[100] = "abc", data[10] = "abc12";
    fprintf(fp,"%s : %s\n",username, data);
}