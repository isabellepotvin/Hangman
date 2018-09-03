/*

Title: Assignment 5 - Hangman
Author: Isabelle Potvin
Date: November 14, 2017

Objective: This program allows users to play a simple game of hangman. The users have the option to play
the game, read instructions on how to play or view highscores. The program loads in 10,000 common medium 
length words and chooses one randomly for each round of the game. The program also loads in previous 
highscores and saves (on exit) any new ones that have been set.


The following references were used to help me build this code
   -> https://www.wikihow.com/Play-Hangman //hangman rules
   -> https://stackoverflow.com/questions/15575799/c-how-to-restrict-input-to-one-char-at-a-time //check if user inputs only one character
   -> http://www.cplusplus.com/reference/cctype/isalpha/ //check if user inputs a letter
   -> http://www.cplusplus.com/forum/beginner/28755/ //convert uppercase letter to lowercase
   -> The getDate function was taken and modified from my submission for assignment 3.

*/

#include "User.h"


#pragma region Enums
enum bodyPart {
	stand,
	head,
	body,
	rightArm,
	leftArm,
	rightLeg,
	leftLeg,
	noose,
	gameOver
};

enum States {
	MainMenu,
	Game,
	Instructions,
	Highscores,
	Exit
};
#pragma endregion


//function prototypes
#pragma region Prototypes

void states(vector<string> words, vector<User> &scores);
States mainMenu();
void game(vector<string> words, vector<User> &highscores);
void rules();
void displayHighscores(vector<User> &scores);

bool LowestToHighest(User i, User j);
void sortScores(vector<User> &scores);
void saveScore(vector<User> &scores, int score);
string getDate();

string randomWord(vector<string> words);
void drawMan(bodyPart &part);
void drawBox();
void gotoXY(int x, int y);
void textColour(int colour);

#pragma endregion




int main() {

	//variables
	vector<string> words;
	string temp;
	vector<User> hScores;
	string tempName;
	string tempDate;
	int tempScore;
	int tempRank;

	srand(time(NULL)); //seeds random
	textColour(15); //changes the text to white with a black background

	ifstream dictionary("Dictionary.txt"); //opens file
	ifstream highscores("Highscores.txt"); //opens file

	if (!dictionary.fail() && !highscores.fail()) { //checks that the filse have opened correctly

		//saves all the words from the dictionary file
		while (!dictionary.eof()) {
			getline(dictionary, temp); //gets the next word from the file
			words.push_back(temp); //saves the word into the vector
		}

		//saves all the info from the highscores file
		while (!highscores.eof()) {
			highscores >> tempRank;
			highscores >> tempName;
			highscores >> tempScore;
			highscores >> tempDate;

			hScores.push_back(User(tempRank, tempName, tempScore, tempDate)); //saves the information as a User into the vector
		}
		dictionary.close(); //closes file
		highscores.close(); //closes file

		//calls the menu function
		states(words, hScores);


		//saves the highscores to a file

		ofstream saveScores("Highscores.txt"); //opens file

		sortScores(hScores); //sorts the scores

		if (!saveScores.fail()) {

			for (int i = 0; i < hScores.size(); i++) {
				hScores[i].saveInfo(&saveScores); //saves each user's info to a line in the file
			}

			saveScores.close(); //closes file
		}

		else {
			//displays error message if the file does not open correctly
			system("cls");
			cout << "The file has failed to open" << endl;
			system("pause");
		}



	}
	else {
		//displays error message if a file does not open correctly
		system("cls");
		cout << "A file has failed to open" << endl;
		system("pause");
	}


	return 0;
}





/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
							STATE RELATED FUNCTIONS
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma region STATES

//this function switches through the states
void states(vector<string> words, vector<User> &scores) {

	//variables
	States currentState = MainMenu;

	//switches through the different states until the user decides to 'exit'
	do {
		switch (currentState) {
		//MAIN MENU
		case MainMenu:
			system("cls");
			currentState = mainMenu(); //calls function that displays the main menu
			break;

		//GAME
		case Game:
			system("cls");
			game(words, scores); //calls function that runs through the game
			currentState = MainMenu; //returns to main menu after user is done the game
			break;

		//DISPLAYS INSTRUCTIONS
		case Instructions:
			system("cls");
			rules(); //calls function that displays the instructions on how to play
			currentState = MainMenu; //returns to main menu after user presses enter
			break;

		//DISPLAYS HIGHSCORES
		case Highscores:
			system("cls");
			displayHighscores(scores); //calls function that displays the highscores
			currentState = MainMenu; //returns to main menu after user presses enter
			break;

		//DEFAULT (invalid state)
		default:
			system("cls");
			cout << "Invalid state";
			system("pause");
			break;
		}
	} while (currentState != Exit);

	//displays exit message
	system("cls");
	gotoXY(40, 10);
	cout << "Thanks for playing! Goodbye.";
	gotoXY(40, 12);
	system("pause");
}

//this function displays the main menu and asks the user to select a state
States mainMenu() {

	//variables
	string input;
	int choice;
	int x = 50;
	int y = 10;
	bool isValid;
	bodyPart indexPart;

	//draws robot
	for (int i = 0; i < 8; i++) {
		indexPart = (bodyPart)i;
		drawMan(indexPart);
	}

	//displays welcome message
	gotoXY(x, y - 4);
	cout << "*  *  *  *   Welcome to HangRobot   *  *  *  *";

	//displays menu options
	gotoXY(x, y);
	cout << "Please select one of the following options:";
	gotoXY(x, y + 2);
	cout << " 1. Play Game";
	gotoXY(x, y + 3);
	cout << " 2. How to Play";
	gotoXY(x, y + 4);
	cout << " 3. Highscores";
	gotoXY(x, y + 5);
	cout << " 4. Exit";


	//asks user to enter their choice
	do {

		isValid = true;

		gotoXY(x, y + 8);
		cout << "Enter your selection:                                                 ";
		gotoXY(x + 22, y + 8);
		getline(cin, input);



		//checks for invalid choice
		if (input.size() == 1) {

			choice = input[0] - 48; //saves input as an int

			//checks if the number is between 1 and 4
			if (choice < 1 || choice > 4) {
				isValid = false;
			}

		}
		else { isValid = false; }

		//displays error message
		if (!isValid) {
			gotoXY(x, y + 9);
			cout << "Invalid choice.";
		}

	} while (!isValid); //loops until user enters valid choice

	return (States)choice; //return user's choice

}

//this function runs through the game
void game(vector<string> words, vector<User> &highscores) {

	//variables
	string randWord;
	bodyPart indexBodyPart = stand;
	string sGuess = " ";
	char cGuess[2] = " ";
	bool isValid;
	int numCorrect = 0;
	vector<char> guesses;
	bool correctGuess;
	bool sameGuess;
	int wrongX = 57;
	int wrongY = 5;
	int score = 0;

	//calls function that picks a random word
	randWord = randomWord(words);


	drawBox(); //draws the box that will display the incorrect letters

	//draws a line for each letter in the word
	gotoXY(60, 13);
	for (int i = 0; i < randWord.size(); i++) {
		cout << (char)95 << (char)255;
	}

	do {
		//asks user to input a letter
		do {
			isValid = false;
			sameGuess = false;


			gotoXY(55, 16);
			cout << "Please enter a letter:                                 "; //prompts user
			gotoXY(78, 16);
			getline(cin, sGuess); //saves user input

								  //checks if the user enters a single letter
			if (sGuess.size() == 1) { //checks if the user only enters ONE character
				cGuess[0] = sGuess[0];

				if (isalpha(cGuess[0])) { //checks if the character is a LETTER

					cGuess[0] = tolower(cGuess[0]); //converts the letter to a lowercase letter

													//checks if the user has already guessed the letter
					for (int i = 0; i < guesses.size(); i++) {
						if (guesses[i] == cGuess[0]) {
							sameGuess = true;
						}
					}

					//if the user hasn't already guessed the letter, then the input is valid
					if (!sameGuess) {
						isValid = true;
						guesses.push_back(cGuess[0]);
					}
					else { //displays error message if the user's entry is invalid
						gotoXY(55, 18);
						cout << "                                                       ";
						gotoXY(55, 18);
						cout << "Invalid entry, you have already guessed '" << cGuess[0] << "'";
					}
				}
				else { //displays error message if the user's entry is invalid
					gotoXY(55, 18);
					cout << "                                                       ";
					gotoXY(55, 18);
					cout << "Invalid entry, you must enter a letter";
				}
			}
			else { //displays error message if the user's entry is invalid
				gotoXY(55, 18);
				cout << "                                                       ";
				gotoXY(55, 18);
				cout << "Invalid entry, you must enter a single character";
			}

		} while (!isValid); //keeps running loop until the user enters a single letter

		//if the guess is correct
		correctGuess = false;
		for (int i = 0; i < randWord.size(); i++) {
			if (cGuess[0] == randWord[i]) {
				gotoXY(55, 18);
				cout << "                                                       ";
				gotoXY(55, 18);
				cout << "'" << cGuess[0] << "' is a correct guess!";

				// reveals/displays letter in the secret word
				gotoXY((60 + 2 * i), 13);
				cout << cGuess[0];

				numCorrect++;
				correctGuess = true;
			}
		}

		//if the guess is incorrect
		if (!correctGuess) {

			gotoXY(55, 18);
			cout << "                                                  ";
			gotoXY(55, 18);
			cout << "Sorry, '" << cGuess[0] << "' is not in the word";

			//displays the letter in the'Incorrect Letters' box
			gotoXY(wrongX, wrongY);
			cout << cGuess[0];
			wrongX += 2;
			score++;

			drawMan(indexBodyPart); //draws the next body part
		}


	} while (indexBodyPart != gameOver && numCorrect != randWord.size());

	//clears message
	gotoXY(55, 18);
	cout << "                                                  ";

	textColour(240); //changes the text to black with a white background

	if (numCorrect == randWord.size()) {
		gotoXY(55, 18);
		cout << "Congratulations! You have won the game.";
		gotoXY(55, 19);
		cout << "It took you " << score << " wrong guess(es) to uncover the word '" << randWord << "'.";;
	}
	else {
		gotoXY(55, 18);
		cout << "Sorry, you have lost the game.";
		gotoXY(55, 19);
		cout << "The correct word was '" << randWord << "'.";
	}

	gotoXY(55, 21);
	system("pause");

	textColour(15); //changes the text to white with a black background

	//checks if the user has set a highscore
	saveScore(highscores, score);

}

//this function displays the rules of the game
void rules() {

	int x = 25;
	int y = 1;

	//displays page title
	gotoXY(x, y);
	cout << "*  *  *  *  *  *  *  *  *  *  *   HOW TO PLAY   *  *  *  *  *  *  *  *  *  *  *";

	//displays first rule
	gotoXY(x, y + 3);
	cout << "1 - The game will choose a random word and display a blank (underscore) for ";
	gotoXY(x, y + 4);
	cout << "    each letter of the word.";

	//displays second rule
	gotoXY(x, y + 6);
	cout << "2 - The player must then start guessing letters they think might belong";
	gotoXY(x, y + 7);
	cout << "    to the word. If the guess is correct, the letter will appear on the ";
	gotoXY(x, y + 8);
	cout << "    screen in place of the corresponding blank. If the guess is incorrect,";
	gotoXY(x, y + 9);
	cout << "    the letter will appear in the 'Incorrect Letters' box.";

	//displays third rule
	gotoXY(x, y + 11);
	cout << "3 - For every wrong guess, a part of the 'hangrobot' will be drawn. The ";
	gotoXY(x, y + 12);
	cout << "    game will draw the stand, the head, the body, the right arm, the left";
	gotoXY(x, y + 13);
	cout << "    arm, the right leg, the left leg, and then the noose. If the noose gets ";
	gotoXY(x, y + 14);
	cout << "    drawn, the player has lost the game.";

	//displays fourth rule
	gotoXY(x, y + 16);
	cout << "4 - The player wins the game when they guess the correct word. The goal of ";
	gotoXY(x, y + 17);
	cout << "    the game is to uncover the entire word in the least amount of guesses.";

	//explains how the game keeps score
	gotoXY(x, y + 19);
	cout << "Highscores:";
	gotoXY(x, y + 20);
	cout << "  A player's score is equal to the amount of wrong letters they guessed. ";
	gotoXY(x, y + 21);
	cout << "  The players with the lowest scores will have the highest ranking. The game";
	gotoXY(x, y + 22);
	cout << "  will save the scores of the players who've reached the top ten best scores ";
	gotoXY(x, y + 23);
	cout << "  in the game's history.";


	gotoXY(x, 28);
	system("pause");
}

//this function displays the highscores
void displayHighscores(vector<User> &scores) {

	int x = 30;
	int y = 7;

	sortScores(scores); //sorts the highscores

	//displays page title
	gotoXY(x, y - 5);
	cout << "*  *  *  *  *  *   HIGHSCORES   *  *  *  *  *  *";

	//displays column titles
	textColour(11); //changes the text colour to turquoise
	gotoXY(x, y - 2);
	cout << setw(7) << left << "Rank";
	cout << setw(22) << "Name";
	cout << setw(8) << "Score";
	cout << setw(10) << "Date";
	textColour(15); //changes the text to white

	//displays list of users and their scores
	for (int i = 0; i < scores.size(); i++) {
		scores[i].displayInfo(x, y + i * 2);
	}

	gotoXY(x, 28);
	system("pause");

}

#pragma endregion





/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
							SCORE RELATED FUNCTIONS
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma region SCORE

//this function is used in the sort function to sort the scores from lowest to highest
bool LowestToHighest(User i, User j) {
	return (i.getScore() < j.getScore());
}

//this function sorts the scores from lowest to highest
void sortScores(vector<User> &scores) {
	sort(scores.begin(), scores.end(), LowestToHighest); //sorts the scores in descending order 

	if (scores.size() > 10) {
		scores.pop_back(); //removes the lowest score if there are more than 10 scores
	}

	//sets the rank for each player
	for (int i = 0; i < scores.size(); i++) {
		scores[i].setRank(i + 1);
	}

}

//this function prompts the user to save their score if they have set a new highscore
void saveScore(vector<User> &scores, int score) {
	system("cls");

	//variables
	bool save = false;
	string name;


	if (scores.size() >= 10) { //if there are more than 10 highscores saved
		for (int i = 0; i < scores.size(); i++) {
			if (scores[i].getScore() > score) { //checks if the player got a lower score than one of the scores saved
				save = true; 
			}
		}
	}
	else if (score < 8) { //else if the player has won the game
		save = true;
	}

	if (save) {

		//prompts the user to enter their name
		do {
			gotoXY(30, 10);
			cout << "Congratulations! You've set a new highscore.";

			gotoXY(30, 12);
			cout << "Please enter your name (less than 20 characters): ";
			getline(cin, name);
			if (name.size() > 20) { //displays error message if the player enters a name that is longer than 20 characters
				system("cls");
				gotoXY(30, 13);
				cout << "Please enter a name that is less than 20 characters.";
			}
			else {
				gotoXY(30, 13);
				cout << "                                                      ";
			}
		} while (name.size() > 20 || name.size() < 1); //loops until the player enters a name that is between 1 and 20 characters

		
		User temp(name, score); //creates a user with the information provided
		scores.push_back(temp); //saves the user in the scores vector

		sortScores(scores); //sorts the scores in descending order

		//displays the information that has been saved
		gotoXY(30, 15);
		cout << "Your score has been saved as: ";

		gotoXY(23, 17);
		textColour(11); //changes the text colour to turquoise
		cout << setw(7) << left << " ";
		cout << setw(22) << "Name";
		cout << setw(8) << "Score";
		cout << setw(10) << "Date";
		textColour(15); //changes the text to white with a black background

		temp.displayInfo(23, 18);

		gotoXY(23, 18);
		cout << setw(7) << left << "      ";

		gotoXY(30, 20);
		system("pause");
	}
}

//this function returns the current date
string getDate() {

	//variables
	int day, month, year;
	string date;

	time_t currentTime = time(NULL);
	struct tm time;
	localtime_s(&time, &currentTime);

	day = time.tm_mday; //current day
	year = time.tm_year + 1900; //current year
	month = time.tm_mon + 1; //current month (stores an number from 0-11)


//saves the date in a string

	//saves the day
	if (day < 10) { date = '0' + to_string(day); } //adds a zero before the number if the day is between 1-9
	else { date = to_string(day); }
	date += '/';
	//saves the month
	if (month < 10) { date += '0' + to_string(month); } //adds a zero before the number if the month is between 1-9
	else { date += to_string(month); }
	date += '/';
	//saves the year
	date += to_string(year);

	return date; //returns the string
}

#pragma endregion





/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
								OTHER FUNCTIONS
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma region OTHER

//this function returns a random word from the list
string randomWord(vector<string> words) {
	int random = rand() % words.size(); //picks random number
	return (words[random]); //returns random word
}

//this function draws the hangman
void drawMan(bodyPart &part) {

	//variables
	int x = 10;
	int y = 2;

	//draws the next part of the hangman and 'increments' the part variable
	switch (part) {

	//draws the STAND
	case stand:
		gotoXY(x + 3, y);
		cout << (char)218 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196;
		for (int i = 0; i < 18; i++) {
			gotoXY(x + 3, y + 1 + i);
			cout << (char)179;
		}
		gotoXY(x, y + 19);
		cout << (char)196 << (char)196 << (char)196 << (char)193 << (char)196 << (char)196 << (char)196;

		part = head;
		break;

	//draws the HEAD
	case head:
		gotoXY(x + 10, y + 2);
		cout << (char)218 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)191;
		gotoXY(x + 10, y + 3);
		cout << (char)179 << (char)255 << (char)248 << (char)255 << (char)248 << (char)255 << (char)179; //eyes
		gotoXY(x + 10, y + 4);
		cout << (char)179 << (char)255 << (char)196 << (char)196 << (char)196 << (char)255 << (char)179; //mouth
		gotoXY(x + 10, y + 5);
		cout << (char)192 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)217;

		part = body;
		break;

	//draws the BODY
	case body:
		gotoXY(x + 13, y + 5);
		cout << (char)194;
		for (int i = 0; i < 5; i++) {
			gotoXY(x + 13, y + (6 + i));
			cout << (char)179;
		}

		part = rightArm;
		break;

	//draws the RIGHT ARM
	case rightArm:
		gotoXY(x + 8, y + 7);
		cout << (char)218 << (char)196 << (char)196 << (char)196 << (char)196 << (char)180;
		for (int i = 0; i < 2; i++) {
			gotoXY(x + 8, y + 8 + i);
			cout << (char)179;
		}

		part = leftArm;
		break;

	//draws the LEFT ARM
	case leftArm:
		gotoXY(x + 13, y + 7);
		cout << (char)197 << (char)196 << (char)196 << (char)196 << (char)196 << (char)217;
		for (int i = 0; i < 2; i++) {
			gotoXY(x + 18, y + 6 - i);
			cout << (char)179;
		}

		part = rightLeg;
		break;

	//draws the RIGHT LEG
	case rightLeg:
		gotoXY(x + 10, y + 11);
		cout << (char)218 << (char)196 << (char)196 << (char)217;
		for (int i = 0; i < 3; i++) {
			gotoXY(x + 10, y + 12 + i);
			cout << (char)179;
		}
		gotoXY(x + 9, y + 15);
		cout << (char)196 << (char)217;

		part = leftLeg;
		break;

	//draws the LEFT LEG
	case leftLeg:
		gotoXY(x + 13, y + 11);
		cout << (char)193 << (char)196 << (char)196 << (char)191;
		for (int i = 0; i < 3; i++) {
			gotoXY(x + 16, y + 12 + i);
			cout << (char)179;
		}
		gotoXY(x + 16, y + 15);
		cout << (char)192 << (char)196;

		part = noose;
		break;


	//draws the NOOSE
	case noose:
		gotoXY(x + 13, y);
		cout << (char)191;
		gotoXY(x + 13, y + 1);
		cout << (char)179;
		gotoXY(x + 13, y + 2);
		cout << (char)193;

		part = gameOver;
		break;

	default:
		cout << "Invalid" << endl;
		break;
	}

}

//this function draws the box for incorrect letters
void drawBox() {

	int x = 55;
	int y = 3;

	//displays title of the box
	gotoXY(x + 1, y);
	cout << "Incorrect Letters";

	//draws the top of the box
	gotoXY(x, y + 1);
	cout << (char)218 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)191;

	//draws the sides of te box
	for (int i = 0; i < 1; i++) {
		gotoXY(x, y + 2 + i);
		cout << (char)179;
		gotoXY(x + 18, y + 2 + i);
		cout << (char)179;
	}

	//draws the bottom of the box
	gotoXY(x, y + 3);
	cout << (char)192 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)217;

}

//this function moves the cursor to a specific coordinate
void gotoXY(int x, int y)
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position = { x, y };
	SetConsoleCursorPosition(hStdout, position);
	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.bVisible = false;
	cursorInfo.dwSize = 10;
	SetConsoleCursorInfo(hStdout, &cursorInfo);
}

//this function changes the colour of the text
void textColour(int colour) {
	HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(ConsoleHandle, colour);
}

#pragma endregion
