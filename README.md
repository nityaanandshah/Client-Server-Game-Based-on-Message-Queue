# Client-Server Game Based on Message Queue

## Game Description

It is a client-server-based game using a message queue.

1. We have a set of k clients and one single server. The server is responsible for creating and maintaining the message queue.

2. At the beginning of each round, the server accepts two integers MIN and MAX from the user, which it sends to all the clients using the message queue.

3. Each client j randomly generates a guessed token (an integer) within the range {MIN:MAX} and sends it to the server, denoted by Rj.

4. The server also generates a token, say G (which is also an integer), independently in the range {MIN:MAX} (not shared with the clients).

5. On receiving Rj from all the clients, the server computes δ = |Rj−G| / ∑(j=1 to j=k) (Rj−G)^2; ∀j ∈ k.

6. Among all the clients, the client scoring the smallest δ wins the round and gets a score of 5.

7. The first client crossing the total score of 50 over multiple rounds is declared as the champion, which ends the game.

## Standard Libraries Required

- stdlib.h
- string.h
- unistd.h
- sys/types.h
- sys/ipc.h
- sys/msg.h
- stdio.h
- time.h
- signal.h

## Running the Program

1. Launch Terminal (Bash) and change the directory to the path of the unzipped folder.

   `cd "path/to/the/file"`

2. Enter the following command in the Bash terminal:
   
   gcc gameclient.c -o gameclient
   ./gameclient
   
   gcc gameserver.c -o gameserver
   ./gameserver
   

Alternatively, you can directly use `./run.sh` to compile and execute the program.

3. Provide the input k (number of clients) and two numbers corresponding to each client (min and max). You can create new clients as desired, as there is no defined upper bound in this server (k).

For each round, the following information is displayed:
- The user-defined range (min - max)
- The value of the received token from the clients (client-generated)
- The current score for each round (the winner array maintains the score)
- The winner for the round with the round number
- The champion of the game

## Implementation Details

### SERVER

1. One server has k number of clients. Using `fork()`, I created k clients and stored the pid of each client in an array, `child_id[]`.

2. Using the `child_id[]`, I created a message queue for each client using `msgget`. I stored the IDs of message queues created in an array, `msgqueue_id[]`.

3. The game executes while there is no winner. Using `msgsnd`, I sent two numbers (min and max) to each client one by one. The client-generated number is received by the server via the message queue and stored in an array `client_generated[]` using `msgrcv`.

4. The server sends messages of type 1 and receives messages of type 2.

5. `guess` is the number the server generated using the `rand()` function. On close inspection of the formula, I deduced that the denominator will remain the same for every client; therefore, calculating the numerator is enough to find the winner.

6. Using `guess`, `roundNum`, `mid`, `winner[]`, and `winner[mid]`, I displayed all the details mentioned in the question.

7. After the game ends, I killed the client processes as they are no longer required using `kill(child_id[], SIGKILL)`.

### CLIENT

1. Each client is associated with a single message queue. I extracted the pid of the client and used it to create a message queue.

2. While the game is running, the client receives two numbers (min and max). It generates a random number in the range of min and max using `rand()` and a seed for random using `time_t` for unique guesses.

3. The client receives messages of type 1 and sends messages of type 2 to the message queue.

## References

- [man7.org](https://man7.org)
- [linux.die.net](https://linux.die.net)
- [ibm.com](https://www.ibm.com)
- [tutorialspoint.com](https://www.tutorialspoint.com)
- [geeksforgeeks.org](https://www.geeksforgeeks.org)

Implementation understood by discussing with Sumit Kumar Prajapati (B20CS074)
