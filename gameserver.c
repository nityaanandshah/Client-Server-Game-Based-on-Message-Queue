#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>

struct message
{
    long int msg_type;
    int data;
} msg;

int main()
{
    // seed for random
    time_t t;
    srand((int)time(&t) % getpid());
    int k;
    fprintf(stdout, "Enter number of clients : ");
    fscanf(stdin, "%d", &k);

    // Obtaining pid of server to create a message queue for each client
    key_t key;
    key = getpid();
    if (key == -1)
    {
        fprintf(stdout, "Error in obtaining pid of server\n");
        perror("getpid");
        exit(1);
    }

    // Creating k number of clients
    int child_id[k];
    for (int i = 0; i < k; i++)
    {
        int t = fork();
        if (t == 0)
        {
            execl("./gameclient", "./gameclient", NULL);
            perror("execl:");
            exit(1);
        }
        else
        {
            child_id[i] = t;
        }
    }

    // Creating message queue for each client
    int msgqueue_id[k];
    for (int i = 0; i < k; i++)
    {
        int msgq_id = msgget(child_id[i], 0644 | IPC_CREAT);
        msgqueue_id[i] = msgq_id;
        if (msgq_id == -1)
        {
            fprintf(stdout, "Error in creating message queue\n");
            perror("msgget");
            exit(1);
        }
    }

    int roundNum = 0;
    int winner[k];
    for (int i = 0; i < k; i++)
    {
        winner[i] = 0;
    }
    while (1)
    {
        int min, max;
        // Send min and max values from server to client via message queue
        fprintf(stdout, "Enter min :\n");
        scanf("%d", &min);
        fprintf(stdout, "Enter max :\n");
        scanf("%d", &max);
        for (int i = 0; i < k; i++)
        {
            msg.msg_type = 1;
            msg.data = min;
            if (msgsnd(msgqueue_id[i], &msg, sizeof(int), 0) == -1)
            {
                perror("msgsnd");
            }

            msg.msg_type = 1;
            msg.data = max;
            if (msgsnd(msgqueue_id[i], &msg, sizeof(int), 0) == -1)
            {
                perror("msgsnd");
            }
        }

        // Recieve the generated number from client via message queue
        int client_generated[k];
        for (int i = 0; i < k; i++)
        {
            long int msg_to_rcv = 2;
            if (msgqueue_id[i] == -1)
            {
                fprintf(stdout, "Error in recieving message from message queue\n");
            }
            msgrcv(msgqueue_id[i], (void *)&msg, sizeof(int), msg_to_rcv, 0);
            printf("Recieved data : %d\n", msg.data);
            client_generated[i] = msg.data;
        }

        // Determining the scores
        int scores[k];
        int guess = rand() % (max - min + 1) + min;
        int mscore = 0;
        int mid = 0;

        for (int i = 0; i < k; i++)
        {
            scores[i] = abs(guess - client_generated[i]);
            if (mscore >= scores[i])
            {
                mscore = scores[i];
                mid = i;
            }
        }
        printf("server guessed %d\n", guess);
        printf("winner of round %d: client #%d\n", roundNum, mid);
        for (int i = 0; i < k; i++)
        {
            printf("%d ", winner[i]);
        }
        printf("\n");
        
        // Determining the winner
        winner[mid] += 5;
        if (winner[mid] == 50)
        {
            printf("%d won\n", mid);
            break;
        }
        roundNum++;
    }

    // Killing the client after game ends
    for (int i = 0; i < k; i++)
    {
        kill(child_id[i], SIGKILL);
    }
    return 0;
}