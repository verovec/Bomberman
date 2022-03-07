#include "../headers/main.h"

void *connectionHandler(void *gameVoid)
{
    gameStruct *game = (gameStruct*)gameVoid;
    int socketNum = game->sockets->currentNumOfSocket;
	int the_socket = game->sockets->clientSockets[socketNum];
	int n;
    int messageInt;
    int playersThatAreHere[4] = {0,0,0,0};
    Uint32 seconds = SDL_GetTicks();

    srand(time(NULL));   // Initialization of time for the rand()
    
    for(int i = 0; i < 4; i++)
    {
        if (game->sockets->clientSockets[i] >= 0) {
            playersThatAreHere[i] = 1;
        }
    }
    
    
	// envoyer le numero du player suivi des players presents
    char *message = malloc(sizeof(char) * 16);
    memset(message, '\0', 16);
	sprintf(message, "%d%d%d%d%d%d", socketNum, playersThatAreHere[0], playersThatAreHere[1], playersThatAreHere[2], playersThatAreHere[3], game->mapCaracteristics->mapNumber);
	printf("--map number : %d\n\n", game->mapCaracteristics->mapNumber);
    write(the_socket , message, 16);
    memset(message, '\0', 16);

    // playerNumberChangeMonitor(game, the_socket);
    char *command = malloc(sizeof(char) * 512);
    memset(command, '\0', 512);
    concatenateGameStat(game, command);
    write(the_socket , command, 512);
    free(command);

	while( (n = recv(the_socket , message , 16, MSG_DONTWAIT)) != 0 ) // replaced read by recv because non bloquant with flag
	{
        if (game->players[socketNum]->playerStats->lives) {
            if (message[0] != '\0') {
                messageInt = atoi(message);
                eventServer(game, messageInt, &seconds, socketNum);
            }
        }
        bombeExplosionDuration(game, socketNum);
        memset(message, '\0', 16);
	}
	
	if(n == 0)
	{
		puts("Client disconnected");
	}
	else if(n == -1)
	{
        puts("Failed message reception");
		perror("read()");
	}

    // reset the socket int the sockets array
    game->sockets->clientSockets[socketNum] = -1;

    if(pthread_detach(pthread_self()) != 0){
        puts("problem terminating thread!");
    } else {
        puts("terminated thread succesfully!");
        game->sockets->maxConnexions--;
    }
}


void *theServer(void *gameVoid){
    int i = 0;    
    gameStruct *game = (gameStruct*)gameVoid;  
    int port = game->infos->portInt;
    int n;
    int connectToServer;
    int the_sockets[4];
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;

    server_addr.sin_addr.s_addr = inet_addr(game->infos->ipAdresse+1);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    int the_socket = socket(AF_INET, SOCK_STREAM, 0);
    int client_socket;
    char message[16];

    if (the_socket < 0) {
        perror ("socket()");
        //return -1;
    }
    if (bind(the_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)  {
        perror ("bind()");
        //return 1;
    }
    if (listen(the_socket, 5) < 0)  {
        perror ("listen()");
        //return 1;
    }
    memset(message, '\0', 16);
    while((client_socket = accept(the_socket, (struct sockaddr*)&client_addr, &client_addr_len))){
        if(game->sockets->maxConnexions < 4) {
            i = 0;
            while(game->sockets->clientSockets[i] >= 0){
                i++;
            }
            game->sockets->currentNumOfSocket = i;
            //printf("this is the num of the socket : %d\n", game->sockets->currentNumOfSocket);
            game->sockets->clientSockets[i] = client_socket;
            puts("new client");
            pthread_t sniffer_thread;
            
            if( pthread_create( &sniffer_thread, NULL,  connectionHandler, (void*)game) < 0)
            {
                perror("thread creation problem");
            } else {
                puts("thread created");
                game->sockets->maxConnexions++;
            } 
        } else {
            close(client_socket);
            puts("too many clients");
        }
    }   
    if (client_socket < 0) {
        perror("accept()");
    }
    close(the_socket);
}
