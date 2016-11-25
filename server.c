#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

struct message{
    long mtype;
    char whom[20];
    char mesg[20];
};

int main(){
    char mass_clnt[100][20];
    int count_clnt = 0;
    key_t token1 = ftok("./server.c",'A');
    key_t token2 = ftok("./server.c",'B');
    int q_serv = msgget(token1, IPC_CREAT | 0666);
    int q_clnt = msgget(token2, IPC_CREAT | 0666);

    while(1){
    struct message buf;

    msgrcv(q_serv, &buf, sizeof(buf), 0, 0);
    if(strcmp(buf.whom, "0") == 0){
        strcpy(mass_clnt[count_clnt],buf.mesg);
        printf("Клиент:%s\n",mass_clnt[count_clnt]);
        count_clnt++;
    }
    if(strcmp(buf.mesg, "exit") == 0){
        msgctl(q_serv, IPC_RMID, 0);
        msgctl(q_clnt, IPC_RMID, 0);
        return 0;
    }
}


    return 0;
}
