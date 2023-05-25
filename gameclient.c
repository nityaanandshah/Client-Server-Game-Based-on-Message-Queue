#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <stdio.h>

struct message{
	long int msg_type;
	int data;
} msg;

int main(){
    // Seed for random
    time_t t;
    srand((int)time(&t) % getpid());

    // Extracting pid of client
    key_t key;
    key = getpid();
    if(key==-1){
        perror("getpid");
        exit(1);
    }

    // Creating message queue
    int msgq_id = msgget(key, 0644 | IPC_CREAT);
    if(msgq_id==-1){
        perror("msgget");
        exit(1);
    }

    int min, max;
    if(msgq_id==-1){
        fprintf(stdout, "Error in creating message to message queue\n");
        exit(1);
    }

    while(1){
        // receiving min and max
        int msg_type =1;
        if(msgrcv(msgq_id, (void *)&msg, sizeof(int), msg_type, 0)==-1){
            perror("msgrcv");
            exit(1);
        }
        min=msg.data;
        // printf("min = %d\n",msg.data);
        
        if(msgrcv(msgq_id, (void *)&msg, sizeof(int), msg_type, 0)==-1){
            perror("msgrcv");
            exit(1);
        }
        max = msg.data;
        // printf("max = %d\n",msg.data);

        // send number generated to server via message queue
        int client_guess = rand() % (max - min + 1) + min;
        msg.data = client_guess;
        msg.msg_type = 2;
        // printf("sending %d from client\n",client_guess);
        if(msgsnd(msgq_id, (void*)&msg, sizeof(int), 0)==-1){
            perror("client msgsnd");
        }
    }
    return 0;
}