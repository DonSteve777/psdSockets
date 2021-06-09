#include "serverGame.h"
#include <pthread.h>

void showError(const char *msg){
	perror(msg);
	exit(0);
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
	struct sockaddr_in player1Address;	/** Client address structure for player 1 */
	struct sockaddr_in player2Address;	/** Client address structure for player 2 */
	int socketPlayer1;					/** Socket descriptor for player 1 */
	int socketPlayer2;					/** Socket descriptor for player 2 */
	unsigned int clientLength;			/** Length of client structure */
	tThreadArgs *threadArgs; 			/** Thread parameters */
	pthread_t threadID;					/** Thread ID */

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

/*--------------------------------------------------------------
player 1 y player 2 se registran
--------------------------------------------------------------*/
		// Get length of client structure
		tSession session;

		/*--------------------------------------------------------------
		player 1 Accept!
		--------------------------------------------------------------*/
		clientLength = sizeof(player1Address);
		socketPlayer1 = accept(socketfd, (struct sockaddr *) &player1Address, &clientLength);
		if (socketPlayer1 < 0)	showError("ERROR while opening socket player 1");
			// Init and read message
			memset(session.player1Name, 0, STRING_LENGTH);
		/*messageLength = */recv(socketPlayer1, session.player1Name, STRING_LENGTH-1, 0);
		printf("player 1: %s\n", session.player1Name);

		/*--------------------------------------------------------------
		player 2  Accept!
		--------------------------------------------------------------*/
		clientLength = sizeof(player2Address);
		socketPlayer2 = accept(socketfd, (struct sockaddr *) &player2Address, &clientLength);
		if (socketPlayer2 < 0)	showError("ERROR while opening socket player 2");
			// Init and read message
			memset(session.player2Name, 0, STRING_LENGTH);
		/*messageLength = */recv(socketPlayer2, session.player2Name, STRING_LENGTH-1, 0);
		printf("player 2: %s\n", session.player2Name);

		initSession(&session);

//		while (1){		////////////////////////////////////////////////////////////////////////////
/*--------------------------------------------------------------
player 1: BET
--------------------------------------------------------------*/
			unsigned int code = TURN_BET;
			/*messageLength = */send(socketPlayer1, &code, sizeof(unsigned int), 0);
			/*messageLength = */send(socketPlayer1, &session.player1Stack, sizeof(unsigned int), 0);
//player 1:recibir bet
			while(code == TURN_BET){
				/*messageLength = */recv(socketPlayer1, &session.player1Bet,sizeof(unsigned int), 0);
			if (SERVER_DEBUG)	printf("%s bet %u\n", session.player1Name, session.player1Bet );
				if (session.player1Bet < 0 || session.player1Bet > session.player1Stack)
					code = TURN_BET;
				else
					code = TURN_BET_OK;
					/*messageLength = */send(socketPlayer1, &code, sizeof(unsigned int), 0);
					showSentCode(code);
					/*messageLength = */send(socketPlayer1, &session.player1Stack, sizeof(unsigned int), 0);
					if (SERVER_DEBUG) printf("%s\n", " stack p1 sent" );

			}

/*--------------------------------------------------------------
player 2: BET
--------------------------------------------------------------*/
				code = TURN_BET;
				/*messageLength = */send(socketPlayer2, &code, sizeof(unsigned int), 0);
				showSentCode(code);

				/*messageLength = */send(socketPlayer2, &session.player2Stack, sizeof(unsigned int), 0);
				if (SERVER_DEBUG) printf("%s\n", " stack p2 sent" );

	//player 2:recibir bet
				while(code == TURN_BET){
					printf("%s\n", "waiting player 2 bet" );
					/*messageLength = */recv(socketPlayer2, &session.player2Bet,sizeof(unsigned int), 0);
					if (SERVER_DEBUG)	printf("%s bet %u\n", session.player2Name, session.player2Bet );
					if (session.player2Bet < 0 || session.player2Bet > session.player2Stack)
						code = TURN_BET;
					else
						code = TURN_BET_OK;
						/*messageLength = */send(socketPlayer2, &code, sizeof(unsigned int), 0);
						showSentCode(code);

						/*messageLength = */send(socketPlayer2, &session.player2Stack, sizeof(unsigned int), 0);
						if (SERVER_DEBUG) printf("%s\n", " stack p2 sent" );

				}
				code = TURN_PLAY;
				/*messageLength = */send(socketPlayer1, &code, sizeof(unsigned int), 0);
				if (SERVER_DEBUG) printf(" %s", "  p1 : " );
				showSentCode(code);


				code = TURN_PLAY_WAIT;
				/*messageLength = */send(socketPlayer2, &code, sizeof(unsigned int), 0);
				if (SERVER_DEBUG) printf("%s","  p2 " );
				showSentCode(code);

				while(1);



				/*------------------------------------------------------------------------------------
				player 1: TURN_PLAY , jugada actual, deck
				-----------------------------------------------------------------------------------------*/
				// code = TURN_PLAY;
				// /*messageLength = */send(socketPlayer1, &code, sizeof(unsigned int), 0);
				// unsigned int player1Points = calculatePoints(&session.player1Deck);
				// printf("%s %u\n","Points :", player1Points );
				// printf("%s\n","Deck :" );
				// printDeck(&session.player1Deck);
				// /*messageLength = */send(socketPlayer1, &player1Points, sizeof(unsigned int), 0);
				// /*messageLength = */send(socketPlayer1, &session.player1Deck, sizeof(session.player1Deck), 0);
				/*------------------------------------------------------------------------------------
				player 2: TURN_PLAY_WAIT , puntos player 1, deck player 1
				-----------------------------------------------------------------------------------------*/
				// code = TURN_PLAY_WAIT;
				// /*messageLength = */send(socketPlayer2, &code, sizeof(unsigned int), 0);
				// printf("%s %u\n","Points player 1:", player1Points );
				// printf("%s\n","Deck :" );
				// printDeck(&session.player1Deck);
				// /*messageLength = */send(socketPlayer2, &player1Points, sizeof(unsigned int), 0);
				// /*messageLength = */send(socketPlayer2, &session.player1Deck, sizeof(session.player1Deck), 0);


/*------------------------------------------------------------------------------------
player 1: STAN / HIT
-----------------------------------------------------------------------------------------*/
		// 		unsigned int option;
		// 		unsigned int codeRival;
		// 		do{
		// 			/*messageLength = */recv(socketPlayer1, &option, sizeof(unsigned int), 0);
		// 			if (option == TURN_PLAY_HIT){
		// 				//enviar code, puntos y nuevo deck
		// 				unsigned int card = getRandomCard (&session.player1Deck);
		//
		// 				session.player1Deck.cards[session.player1Deck.numCards] = card;
		// 				session.player1Deck.numCards++;
		// 				unsigned int player1Points = calculatePoints(&session.player1Deck);
		// 				if (player1Points > 21)
		// 					code = TURN_PLAY_OUT;
		// 				else
		// 					code = TURN_PLAY;
		// 	//envio a P1
		// 				/*messageLength = */send(socketPlayer1, &code, sizeof(unsigned int), 0);
		// 				/*messageLength = */send(socketPlayer1, &player1Points, sizeof(unsigned int), 0);
		// 				/*messageLength = */send(socketPlayer1, &session.player1Deck, sizeof(session.player1Deck), 0);
		// //envio a P2
		// 				codeRival = TURN_PLAY_WAIT;
		// 				/*messageLength = */send(socketPlayer2, &codeRival, sizeof(unsigned int), 0);
		// 				/*messageLength = */send(socketPlayer2, &player1Points, sizeof(unsigned int), 0);
		// 				/*messageLength = */send(socketPlayer2, &session.player1Deck, sizeof(session.player1Deck), 0);
		// 			}
		// 		}while(option == TURN_PLAY_HIT && code == TURN_PLAY);


//while		}	////////////////////////////////////////////////////////////////////////////
}
