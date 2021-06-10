#include "serverGame.h"
#include <pthread.h>

void showError(const char *msg){
	perror(msg);
	exit(0);
}

int acceptConnection (int socketServer){

	int clientSocket;
	struct sockaddr_in clientAddress;
	unsigned int clientAddressLength;

		// Get length of client address
		clientAddressLength = sizeof(clientAddress);

		// Accept
		if ((clientSocket = accept(socketServer, (struct sockaddr *) &clientAddress, &clientAddressLength)) < 0)
			showError("Error while accepting connection");

		printf("Connection established with client: %s\n", inet_ntoa(clientAddress.sin_addr));

	return clientSocket;
}


void showSentCode (unsigned int code){

	tString string;

		if (SERVER_DEBUG){

			// Reset
			memset (string, 0, STRING_LENGTH);

			switch(code){

				case TURN_BET:
					strcpy (string, "TURN_BET");
					break;

				case TURN_BET_OK:
					strcpy (string, "TURN_BET_OK");
					break;

				case TURN_PLAY:
					strcpy (string, "TURN_PLAY");
					break;

				case TURN_PLAY_HIT:
					strcpy (string, "TURN_PLAY_HIT");
					break;

				case TURN_PLAY_STAND:
					strcpy (string, "TURN_PLAY_STAND");
					break;

				case TURN_PLAY_OUT:
					strcpy (string, "TURN_PLAY_OUT");
					break;

				case TURN_PLAY_WAIT:
					strcpy (string, "TURN_PLAY_WAIT");
					break;

				case TURN_PLAY_RIVAL_DONE:
					strcpy (string, "TURN_PLAY_RIVAL_DONE");
					break;

				case TURN_GAME_WIN:
					strcpy (string, "TURN_GAME_WIN");
					break;

				case TURN_GAME_LOSE:
					strcpy (string, "TURN_GAME_LOSE");
					break;

				default:
					strcpy (string, "UNKNOWN CODE");
					break;
			}

			printf ("Sent: ----%s\n", string);
		}
}

void *threadProcessing(void *threadArgs){

	tSession session;				/** Session of this game */
	int socketPlayer1;				/** Socket descriptor for player 1 */
	int socketPlayer2;				/** Socket descriptor for player 2 */
	tPlayer currentPlayer;			/** Current player */
	int endOfGame;					/** Flag to control the end of the game*/
	unsigned int card;				/** Current card */
	unsigned int code, codeRival;	/** Codes for the active player and the rival */

		// Get sockets for players
		socketPlayer1 = ((tThreadArgs *) threadArgs)->socketPlayer1;
		socketPlayer2 = ((tThreadArgs *) threadArgs)->socketPlayer2;

		// Receive player 1 info


		// Receive player 2 info


		// Init...
		endOfGame = FALSE;

		while (!endOfGame){


		}

		// Close sockets
		close (socketPlayer1);
		close (socketPlayer2);

	return (NULL) ;
}




int main(int argc, char *argv[]){

	int socketfd;						/** Socket descriptor */
	struct sockaddr_in serverAddress;	/** Server address structure */
	unsigned int port;					/** Listening port */
	//struct sockaddr_in player1Address;	/** Client address structure for player 1 */
	//struct sockaddr_in player2Address;	/** Client address structure for player 2 */
	int socketPlayer1;					/** Socket descriptor for player 1 */
	int socketPlayer2;					/** Socket descriptor for player 2 */
//	unsigned int clientLength;			/** Length of client structure */
	tThreadArgs *threadArgs; 			/** Thread parameters */
	pthread_t threadID;					/** Thread ID */
//ALVARO variables
		unsigned int endOfGame = FALSE;
		// Seed
		srand(time(0));
		// Check arguments
		if (argc != 2) {
			fprintf(stderr,"ERROR wrong number of arguments\n");
			fprintf(stderr,"Usage:\n$>%s port\n", argv[0]);
			exit(1);
		}
		// Create the socket
		socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		// Check
	 		 if (socketfd < 0)
	 			showError("ERROR while opening socket");
		// Init server structure
		memset(&serverAddress, 0, sizeof(serverAddress));
		// Get listening port
		port = atoi(argv[1]);
		// Fill server structure
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		serverAddress.sin_port = htons(port);
		// Bind: asocia y reserva un puerto para el socket
		if (bind(socketfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
			showError("ERROR while binding");
		// Listen
		listen(socketfd, 10);	 // 10 = queue limit

		while (1){		////////////////////////////////////////////////////////////////////////////
			socketPlayer1 = acceptConnection(socketfd);
			socketPlayer2 = acceptConnection(socketfd);
			// Allocate memory
			if ((threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs))) == NULL)
				showError("Error while allocating memory");
			threadArgs->socketPlayer1 = socketPlayer1;
			threadArgs->socketPlayer2 = socketPlayer2;

			if (pthread_create(&threadID, NULL, threadProcessing, (void *) threadArgs) != 0)
				showError("pthread_create() failed");
		}
		close (socketfd);
}
