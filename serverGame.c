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
		// Init and read message
			memset(session.player1Name, 0, STRING_LENGTH);
		/*messageLength = */recv(socketPlayer1, &session.player1Name, STRING_LENGTH-1, 0);
		printf("player 1: %s\n", session.player1Name);

		// Receive player 2 info
		if (socketPlayer2 < 0)	showError("ERROR while opening socket player 2");
			// Init and read message
			memset(session.player2Name, 0, STRING_LENGTH);
		/*messageLength = */recv(socketPlayer2, &session.player2Name, STRING_LENGTH-1, 0);
		printf("player 2: %s\n", session.player2Name);

		// Init...
		endOfGame = FALSE;
		initSession(&session);

		while (!endOfGame){
			/*--------------------------------------------------------------
player 1: BET
--------------------------------------------------------------*/
			unsigned int code = TURN_BET;
			/*messageLength = */send(socketPlayer1, &code, sizeof(unsigned int), 0);
			/*messageLength = */send(socketPlayer1, &session.player1Stack, sizeof(unsigned int), 0);
//player 1:recibir bet
			while(code == TURN_BET){
				/*messageLength = */recv(socketPlayer1, &session.player1Bet,sizeof(unsigned int), 0);
				if (session.player1Bet < 0 || session.player1Bet > session.player1Stack)
					code = TURN_BET;
				else
					code = TURN_BET_OK;
					/*messageLength = */send(socketPlayer1, &code, sizeof(unsigned int), 0);
					showSentCode(code);
			}
			session.player1Stack -=  session.player1Bet;

/*--------------------------------------------------------------
player 2: BET
--------------------------------------------------------------*/
				code = TURN_BET;
				/*messageLength = */send(socketPlayer2, &code, sizeof(unsigned int), 0);
				showSentCode(code);
				/*messageLength = */send(socketPlayer2, &session.player2Stack, sizeof(unsigned int), 0);
	//player 2:recibir bet
				while(code == TURN_BET){
					/*messageLength = */recv(socketPlayer2, &session.player2Bet,sizeof(unsigned int), 0);
					if (session.player2Bet < 0 || session.player2Bet > session.player2Stack)
						code = TURN_BET;
					else
						code = TURN_BET_OK;
						/*messageLength = */send(socketPlayer2, &code, sizeof(unsigned int), 0);
						showSentCode(code);
				}
				session.player2Stack -=  session.player2Bet;

/*------------------------------------------------------------------------------------
player 1 STAND / HIT
player 2 WAIT
-----------------------------------------------------------------------------------------*/
				unsigned int option;
				codeRival = TURN_PLAY_WAIT;
				code = TURN_PLAY;
				unsigned int player1Points;
				unsigned int player2Points;

//	player 1: TURN_PLAY
					card = getRandomCard (&session.gameDeck);
					session.player1Deck.cards[session.player1Deck.numCards] = card;
					session.player1Deck.numCards++;
					card = getRandomCard (&session.gameDeck);
					session.player1Deck.cards[session.player1Deck.numCards] = card;
					session.player1Deck.numCards++;
					player1Points = calculatePoints(&session.player1Deck);
					/*messageLength = */send(socketPlayer1, &code, sizeof(unsigned int), 0);
					/*messageLength = */send(socketPlayer1, &player1Points, sizeof(unsigned int), 0);
					/*messageLength = */send(socketPlayer1, &session.player1Deck, sizeof(session.player1Deck), 0);
					printf("\n\n");
					showSentCode(code);
//	 player 2: TURN_PLAY_WAIT
					/*messageLength = */send(socketPlayer2, &codeRival, sizeof(unsigned int), 0);
					/*messageLength = */send(socketPlayer2, &player1Points, sizeof(unsigned int), 0);
					/*messageLength = */send(socketPlayer2, &session.player1Deck, sizeof(session.player1Deck), 0);
					showSentCode(codeRival);
					do{
//player 1 OPTION
					/*messageLength = */recv(socketPlayer1, &option, sizeof(unsigned int), 0);
					showSentCode(option);
						if (option == TURN_PLAY_HIT){
							//enviar code, puntos y nuevo deck
							card = getRandomCard (&session.gameDeck);
							session.player1Deck.cards[session.player1Deck.numCards] = card;
							session.player1Deck.numCards++;
							player1Points = calculatePoints(&session.player1Deck);
							code = player1Points > 21 ? TURN_PLAY_OUT : TURN_PLAY;
							codeRival = code == TURN_PLAY_OUT ? TURN_PLAY_RIVAL_DONE : TURN_PLAY_WAIT;
							/*messageLength = */send(socketPlayer1, &code, sizeof(unsigned int), 0);
							/*messageLength = */send(socketPlayer1, &player1Points, sizeof(unsigned int), 0);
							/*messageLength = */send(socketPlayer1, &session.player1Deck, sizeof(session.player1Deck), 0);
							showSentCode(code);
						}
						else if (option == TURN_PLAY_STAND){
							codeRival = TURN_PLAY_RIVAL_DONE;
						}
		//	 player 2: TURN_PLAY_WAIT
						/*messageLength = */send(socketPlayer2, &codeRival, sizeof(unsigned int), 0);
						/*messageLength = */send(socketPlayer2, &player1Points, sizeof(unsigned int), 0);
						/*messageLength = */send(socketPlayer2, &session.player1Deck, sizeof(session.player1Deck), 0);
						showSentCode(codeRival);
						printf("\n\n");
				}while(option == TURN_PLAY_HIT && code == TURN_PLAY);
/*------------------------------------------------------------------------------------
player 2 STAND / HIT
player 1 WAIT
-----------------------------------------------------------------------------------------*/
					code = TURN_PLAY;
					codeRival = TURN_PLAY_WAIT;
					card = getRandomCard (&session.gameDeck);
					session.player2Deck.cards[session.player2Deck.numCards] = card;
					session.player2Deck.numCards++;
					card = getRandomCard (&session.gameDeck);
					session.player2Deck.cards[session.player2Deck.numCards] = card;
					session.player2Deck.numCards++;
					player2Points = calculatePoints(&session.player2Deck);

					/*messageLength = */send(socketPlayer2, &code, sizeof(unsigned int), 0);
					/*messageLength = */send(socketPlayer2, &player2Points, sizeof(unsigned int), 0);
					/*messageLength = */send(socketPlayer2, &session.player2Deck, sizeof(session.player2Deck), 0);
					printf("\n\n");
					showSentCode(code);
					/*messageLength = */send(socketPlayer1, &codeRival, sizeof(unsigned int), 0);
					/*messageLength = */send(socketPlayer1, &player2Points, sizeof(unsigned int), 0);
					/*messageLength = */send(socketPlayer1, &session.player2Deck, sizeof(session.player2Deck), 0);
					showSentCode(codeRival);
				do{
				/*messageLength = */recv(socketPlayer2, &option, sizeof(unsigned int), 0);
				showSentCode(option);
					if (option == TURN_PLAY_HIT){
						card = getRandomCard (&session.gameDeck);
						session.player2Deck.cards[session.player2Deck.numCards] = card;
						session.player2Deck.numCards++;
						player2Points = calculatePoints(&session.player2Deck);

						code = player2Points > 21 ? TURN_PLAY_OUT : TURN_PLAY;
						codeRival = code == TURN_PLAY_OUT ? TURN_PLAY_RIVAL_DONE : TURN_PLAY_WAIT;

						/*messageLength = */send(socketPlayer2, &code, sizeof(unsigned int), 0);
						/*messageLength = */send(socketPlayer2, &player2Points, sizeof(unsigned int), 0);
						/*messageLength = */send(socketPlayer2, &session.player2Deck, sizeof(session.player2Deck), 0);
						showSentCode(code);

					}
					else if (option == TURN_PLAY_STAND)
						codeRival = TURN_PLAY_RIVAL_DONE;
		//	 player 1: TURN_PLAY_WAIT
						/*messageLength = */send(socketPlayer1, &codeRival, sizeof(unsigned int), 0);
						/*messageLength = */send(socketPlayer1, &player2Points, sizeof(unsigned int), 0);
						/*messageLength = */send(socketPlayer1, &session.player2Deck, sizeof(session.player2Deck), 0);
						showSentCode(codeRival);
					printf("\n\n");
			}while(option == TURN_PLAY_HIT && code == TURN_PLAY);
/*------------------------------------------------------------------------------------
WINNER CHECK
-----------------------------------------------------------------------------------------*/
			updateStacks(&session);
			printSession (&session);
			if (session.player1Stack == 0 ){
				code = TURN_GAME_LOSE;
				codeRival = TURN_GAME_WIN;
				endOfGame = TRUE;
			}
			else if (session.player2Stack == 0 ){
				codeRival = TURN_GAME_LOSE;
				code = TURN_GAME_WIN;
				endOfGame = TRUE;
			}
			else{
				code = SUIT_SIZE;	//UNKN
				codeRival = SUIT_SIZE;

			//	getNextPlayer (currentPlayer);
				clearDeck(&session.player1Deck);
				clearDeck(&session.player2Deck);
				printf("%s\n", "--------------NEXT ROUND----------\n FIGHT!!!!!" );
			}
			/*messageLength = */send(socketPlayer1, &code, sizeof(unsigned int), 0);
			/*messageLength = */send(socketPlayer2, &codeRival, sizeof(unsigned int), 0);

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
			if ((threadArgs = (struct ThreadArgs *) malloc(sizeof( struct ThreadArgs *))) == NULL)
				showError("Error while allocating memory");
			threadArgs->socketPlayer1 = socketPlayer1;
			threadArgs->socketPlayer2 = socketPlayer2;

			if (pthread_create(&threadID, NULL, threadProcessing, (void *) threadArgs) != 0)
				showError("pthread_create() failed");
		}
		close (socketfd);
}
