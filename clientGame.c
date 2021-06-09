#include "clientGame.h"

void showError(const char *msg){
	perror(msg);
	exit(0);
}

void showReceivedCode (unsigned int code){

	tString string;

		if (DEBUG_CLIENT){

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

			printf ("Received: ----%s\n", string);
		}
}

unsigned int readBet (){

	int i, isValid, bet;
	tString enteredMove;

		// Init...
		bet = 0;

		// While player does not enter a correct bet...
		do{

			// Init...
			bzero (enteredMove, STRING_LENGTH);
			isValid = TRUE;

			// Show input message
			printf ("Enter a bet:");

			// Read move
			fgets(enteredMove, STRING_LENGTH-1, stdin);

			// Remove new-line char
			enteredMove[strlen(enteredMove)-1] = 0;

			// Check if each character is a digit
			for (i=0; i<strlen(enteredMove) && isValid; i++){

				if (!isdigit(enteredMove[i]))
					isValid = FALSE;
			}

			// Entered move is not a number
			if (!isValid){
				printf ("Entered bet is not correct. It must be a number greater than 0\n");
			}

			// Entered move is a number...
			else{

				// Conver entered text to an int
				bet = atoi (enteredMove);
			}

		}while (!isValid);

		printf ("\n");

	return ((unsigned int) bet);
}

unsigned int readOption (){

	int i, isValid, option;
	tString enteredMove;

		// Init...
		option = 0;

		// While player does not enter a correct bet...
		do{

			// Init...
			bzero (enteredMove, STRING_LENGTH);
			isValid = TRUE;

			// Show input message
			printf ("Press %d to hit a card and %d to stand:", PLAYER_HIT_CARD, PLAYER_STAND);

			// Read move
			fgets(enteredMove, STRING_LENGTH-1, stdin);

			// Remove new-line char
			enteredMove[strlen(enteredMove)-1] = 0;

			// Check if each character is a digit
			for (i=0; i<strlen(enteredMove) && isValid; i++){

				if (!isdigit(enteredMove[i]))
					isValid = FALSE;
			}

			// Entered move is not a number
			if (!isValid){
				printf ("Wrong option!\n");
			}

			// Entered move is a number...
			else{

				// Conver entered text to an int
				option = atoi (enteredMove);

				if ((option != PLAYER_HIT_CARD) && (option != PLAYER_STAND)){
					printf ("Wrong option!\n");
					isValid = FALSE;
				}
			}

		}while (!isValid);

		printf ("\n");

	return ((unsigned int) option);
}


int main(int argc, char *argv[]){

	int socketfd;						/** Socket descriptor */
	unsigned int port;					/** Port number (server) */
	struct sockaddr_in server_address;	/** Server address structure */
	char* serverIP;						/** Server IP */
	unsigned int endOfGame;				/** Flag to control the end of the game */
	tString playerName;					/** Name of the player */
	unsigned int code;					/** Code */


		// Check arguments!
		if (argc != 3){
			fprintf(stderr,"ERROR wrong number of arguments\n");
			fprintf(stderr,"Usage:\n$>%s serverIP port\n", argv[0]);
			exit(0);
		}

		// Get the server address
		serverIP = argv[1];

		// Get the port
		port = atoi(argv[2]);

		// Create socket
		socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		// Check
		if (socketfd < 0)
		 showError("ERROR while opening socket");

		// Fill server address structure
		memset(&server_address, 0, sizeof(server_address));
			server_address.sin_family = AF_INET;
			server_address.sin_addr.s_addr = inet_addr(serverIP);
			server_address.sin_port = htons(port);

		// Connect with server
		if (connect(socketfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
			showError("ERROR while establishing connection");


		printf ("Connection established with server!\n");

		// Init player's name
		do{
			memset(playerName, 0, STRING_LENGTH);
			printf ("Enter player name:");
			fgets(playerName, STRING_LENGTH-1, stdin);

			// Remove '\n'
			playerName[strlen(playerName)-1] = 0;

		}while (strlen(playerName) <= 2);

		// Init
		/*messageLength = */send(socketfd, playerName, strlen(playerName), 0);

		// Game starts
		printf ("Game starts!\n\n");

		// While game continues...
//		while (!endOfGame){	////////////////////////////////////////////////////////////////////////////
/*--------------------------------------------------------------
							BET
--------------------------------------------------------------*/
			unsigned int stack;
			code = TURN_BET;
			while(code == TURN_BET ){
				if (DEBUG_CLIENT) printf("%s\n", "waiting code" );
				/*messageLength = */recv(socketfd, &code , sizeof(unsigned int), 0);	//TURN_BET
				showReceivedCode(code);
				if (code == TURN_BET){
					/*messageLength = */recv(socketfd, &stack, sizeof(unsigned int), 0);	//STACK
					printf("%s %u\n", "stack: ", stack );
					unsigned int bet = readBet();
					send(socketfd, &bet, sizeof(unsigned int), 0);
				}
			}
			/*------------------------------------------------------------------------------------
			Esperar turno
			-----------------------------------------------------------------------------------------*/
			if (DEBUG_CLIENT)	printf("%s\n", "waiting turn play xxxxxxxxxxxxxxxx" );
			/*messageLength = */recv(socketfd, &code , sizeof(unsigned int), 0);
			showReceivedCode(code);
			unsigned int points;
			tDeck deck;
/*------------------------------------------------------------------------------------
PLAY
-----------------------------------------------------------------------------------------*/
		// 	if (code == TURN_PLAY){
		// 		printf("%s\n", "waiting points and deck" );
		// 		/*messageLength = */recv(socketfd, &points , sizeof(unsigned int), 0);
		// 		/*messageLength = */recv(socketfd, &deck , sizeof(deck), 0);
		// 		printf("%s %u\n","Points :", points );
		// 		printDeck(&deck);
		// 		while (code == TURN_PLAY){
		// 			unsigned int option = readOption();
		// 			/*messageLength = */send(socketfd, &option, sizeof(unsigned int), 0);
		// 			if (option == TURN_PLAY_HIT){
		// 				/*messageLength = */recv(socketfd, &code , sizeof(unsigned int), 0);
		// 				/*messageLength = */recv(socketfd, &points , sizeof(unsigned int), 0);
		// 				/*messageLength = */recv(socketfd, &deck , sizeof(deck), 0);
		// 				showReceivedCode(code);
		// 			}
		// 		}
		// }
//STAND / HIT

/*------------------------------------------------------------------------------------
WAIT
-----------------------------------------------------------------------------------------*/
			// else if (code == TURN_PLAY_WAIT){
			// 	/*messageLength = */recv(socketfd, &points , sizeof(unsigned int), 0);
			// 	/*messageLength = */recv(socketfd, &deck , sizeof(deck), 0);
			// 	printf("%s %u\n","Opponent \n Points :", points );
			// 	printDeck(&deck);
			// }
			// while (code == TURN_PLAY_WAIT){
			// 	/*messageLength = */recv(socketfd, &code , sizeof(unsigned int), 0);
			// 	/*messageLength = */recv(socketfd, &points , sizeof(unsigned int), 0);
			// 	/*messageLength = */recv(socketfd, &deck , sizeof(deck), 0);
			// 	printf("%s %u\n","Opponent \n Points :", points );
			// 	printDeck(&deck);
			// }

			/*------------------------------------------------------------------------------------
			TURN_PLAY_WAIT , jugada actual, deck
			-----------------------------------------------------------------------------------------*/


//		}////////////////////////////////////////////////////////////////////////////

	// Close socket
	close (socketfd);
}
