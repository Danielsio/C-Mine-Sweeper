/********************************************************************************************************************************
NAME : Daniel Sionov
ID : 318670668
Group : 5
this program print out the well known game called "minesweeper". HAVE FUN !!!.
*********************************************************************************************************************************/
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define _CRT_SECURE_NO_WARNINGS

#define MAX_SIZE 22


// this is to get a better access to any square on the game board and change it's value according to the rules.
typedef struct Square {
	int BombsClose;
	bool IsBomb;
	int visibleState;
} Square;

// this is a struct that containing a bool variable, it will help us know if the game is ended. also an int variable to get how many sqaure were opened;
typedef struct Result {
	int SquaresOpened;
	bool gameOver;
} Result;

typedef struct Choice {
	int rows;
	int cols;
	int size_of_; // this is a variable that help me know how many chars of '_' i should print for every size of the game board when i send it to the print-board function;
} Choice;

//functions declerations
void printLine(int count, char ch);
void printWelcome();
void setCloseBombsValue(Square board[][MAX_SIZE], int maxRows, int maxCols);
void setBoardVisibleStateToHide(Square board[][MAX_SIZE], int rows, int cols);
void setBombsBoolianValueToFalse(Square board[][MAX_SIZE], int rows, int cols);
void setBombsOnBoardRandomally(Square board[][MAX_SIZE], int sizeOfRows, int sizeOfCols);
int findBombsClose(Square board[][MAX_SIZE], int row, int col, int maxRows, int maxCols);
void init_board(Square* board, int rows, int cols);
void printBoard(Square board[][MAX_SIZE], int rows, int cols, int countOf_);
void printNextMove();
void uncoverBoard(Square board[][MAX_SIZE], int rows, int cols);
int openSquare(Square board[][MAX_SIZE], int row, int col, int rows, int cols, int SquaresOpened);
int openSquaresAround(Square board[][MAX_SIZE], int row, int col, int rows, int cols, int SquaresOpened);
bool isInBoard(int row, int col, int rows, int cols);
Result game_procedure(Square board[][MAX_SIZE], int row, int col, char inputType, int num_of_rows, int num_of_cols, int size_of_, int SquaresOpened, int num_Of_Bombs, bool gameOver);
Result openNextXCells(Square board[][MAX_SIZE], int col, int num_of_rows, int num_of_cols, int SquaresOpened, int num_Of_Bombs, bool gameOver);
Choice printMenu(Square board[][MAX_SIZE], int rows, int cols);

void main()
{
	Result result;
	result.SquaresOpened = 0;
	int checkInput, row, col, num_of_bombs;
	bool gameOver = false;
	// O or F
	char inputType;
	Square board[MAX_SIZE][MAX_SIZE];
	
	Choice size = printMenu(board, MAX_SIZE, MAX_SIZE);
	if (size.rows == 0)
		return;
	num_of_bombs = (int)sqrt(size.rows * size.cols);
	init_board(&board, size.rows, size.cols);
	printf("\n");

	while (!gameOver)
	{
		if (result.SquaresOpened == (size.rows * size.cols) - num_of_bombs)
		{
			gameOver = true;
			continue;
		}
		printBoard(board, size.rows, size.cols, size.size_of_);

		// to avoid the continuity of the program in this case game ends with victory;
		
		printNextMove();
		checkInput = scanf("%d%d", &row, &col);
		while (!isInBoard(row, col, size.rows, size.cols)) // if input is bad we scan again until we good;
		{
			if (row == -1 && col > 0) // cheaters function this will allow to open x cells on the board for an EZ WIN; also asuming the user know how to get to this function;
			{
				result = openNextXCells(board, col, size.rows, size.cols, result.SquaresOpened, num_of_bombs, gameOver);
				if (result.gameOver == true)
				{
					printf("\n");
					printBoard(board, size.rows, size.cols, size.size_of_);
					break;
				}
				break;
			}
			else 
			{
				printf("please enter a valid choice: ");
				checkInput = scanf("%d%d", &row, &col);
			}
		}
		printf("\n");
		if (isInBoard(row, col, size.rows, size.cols)) // a case of valid input we get to a normal game procedure via the rules of the game;
		{
			checkInput = scanf(" %c", &inputType);
			result = game_procedure(board, row, col, inputType, size.rows, size.cols, size.size_of_, result.SquaresOpened, num_of_bombs, gameOver);
			if (result.gameOver == true) // if user won/lost then we exit the loop;
				break;
		}

		
		else if (row != -1)
		{
			printf("Invalid move, please enter valid choice!\n\n");
		}
	}
	// after game ends, the loop stops and we prints the messege according to the board and the rules;

	// case of victory;
	if (result.SquaresOpened == (size.rows * size.cols) - num_of_bombs)
		printf("You won! congratulations! CHAMPION!\n");
	// case of loss;
	else
	{
		printf("\nYou've hit a bomb! Game over!\n");
	}

}

// this function print a line of chars;
void printLine(int count, char ch)
{
	int i;

	for (i = 0; i < count; i++)
		printf("%c", ch);

	printf("\n");
}
// this function prints the welcome lines and the menu selection options;
void printWelcome()
{
	printf("Welcome to Minesweeper!\n\n");
	printf("Please choose one of the following options and enter it's number:\n\n1 - for size 8X8\n\n2 - for size 12X12\n\n3 - for size 15X15\n\n4 - for custom size\n\n0 - Exit\n\n");
}
// this function sets the board squares to be initialized with the value of how close they are to the bombs;
void setCloseBombsValue(Square board[][MAX_SIZE], int rows, int cols)
{
	for (int k = 0; k < rows; k++)
		for (int l = 0; l < cols; l++)
			board[k][l].BombsClose = findBombsClose(board, k, l, rows, cols);
}
// this function sets the board squares to be initialized to there hidden value which means the user wont see whats behind them until he uncover the square;
void setBoardVisibleStateToHide(Square board[][MAX_SIZE], int rows, int cols)
{
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
		{
			board[i][j].visibleState = 0;
		}
}
// this function sets the bombs to be in random locations on board;
void setBombsOnBoardRandomally(Square board[][MAX_SIZE], int rows, int cols)
{
	int i, j, numOfBombs;
	numOfBombs = (int)sqrt(rows * cols);
	setBombsBoolianValueToFalse(board, rows, cols);

	srand(time(NULL));
	for (int k = 0; k < numOfBombs; k++)
	{
		do {
			i = (rand() % rows);
			j = (rand() % cols);
		} while (board[i][j].IsBomb);
		board[i][j].IsBomb = true;
	}
}
// this function sets the squares of the game board to be all not bombs;  
void setBombsBoolianValueToFalse(Square board[][MAX_SIZE], int rows, int cols)
{
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			board[i][j].IsBomb = false;
}
// this function find out how many bombs is close to a specific square;
int findBombsClose(Square board[][MAX_SIZE], int row, int col, int ROWS, int COLS)
{
	int bombsFound = 0;
	int i = row;
	int j = col;

	// there is only 9 options for a square to be on a diffrent position on the game board so..;

	// option 1
	if (i == 0 && j == 0)
	{
		if (board[i][j + 1].IsBomb == true)
			bombsFound++;
		if (board[i + 1][j].IsBomb == true)
			bombsFound++;
		if (board[i + 1][j + 1].IsBomb == true)
			bombsFound++;
	}
	// option 2
	else if (i == 0 && j > 0)
	{
		if (board[i][j - 1].IsBomb == true)
			bombsFound++;
		if (board[i][j + 1].IsBomb == true)
			bombsFound++;
		if (board[i + 1][j - 1].IsBomb == true)
			bombsFound++;
		if (board[i + 1][j].IsBomb == true)
			bombsFound++;
		if (board[i + 1][j + 1].IsBomb == true)
			bombsFound++;
	}
	// option 3
	else if (i == 0 && COLS - 1 > 0 && j == COLS - 1)
	{
		if (board[i][j - 1].IsBomb == true)
			bombsFound++;
		if (board[i + 1][j - 1].IsBomb == true)
			bombsFound++;
		if (board[i + 1][j].IsBomb == true)
			bombsFound++;
	}
	// option 4
	else if (i > 0 && i < COLS - 1 && j == COLS - 1)
	{
		if (board[i - 1][j - 1].IsBomb == true)
			bombsFound++;
		if (board[i - 1][j].IsBomb == true)
			bombsFound++;
		if (board[i][j - 1].IsBomb == true)
			bombsFound++;
		if (board[i + 1][j - 1].IsBomb == true)
			bombsFound++;
		if (board[i + 1][j].IsBomb == true)
			bombsFound++;
	}
	// option 5
	else if (ROWS - 1 > 0 && i == ROWS - 1 && COLS - 1 > 0 && j == COLS - 1)
	{
		if (board[i - 1][j - 1].IsBomb == true)
			bombsFound++;
		if (board[i - 1][j].IsBomb == true)
			bombsFound++;
		if (board[i][j - 1].IsBomb == true)
			bombsFound++;
	}
	// option 6
	else if (ROWS - 1 > 0 && i == ROWS - 1 && j > 0 && j < COLS - 1)
	{
		if (board[i - 1][j - 1].IsBomb == true)
			bombsFound++;
		if (board[i - 1][j].IsBomb == true)
			bombsFound++;
		if (board[i - 1][j + 1].IsBomb == true)
			bombsFound++;
		if (board[i][j - 1].IsBomb == true)
			bombsFound++;
		if (board[i][j + 1].IsBomb == true)
			bombsFound++;
	}
	// option 7
	else if (ROWS - 1 > 0 && i == ROWS - 1 && j == 0)
	{
		if (board[i - 1][j].IsBomb == true)
			bombsFound++;
		if (board[i - 1][j + 1].IsBomb == true)
			bombsFound++;
		if (board[i][j + 1].IsBomb == true)
			bombsFound++;
	}
	// option 8
	else if (i > 0 && i < ROWS - 1 && j == 0)
	{
		if (board[i - 1][j].IsBomb == true)
			bombsFound++;
		if (board[i - 1][j + 1].IsBomb == true)
			bombsFound++;
		if (board[i][j + 1].IsBomb == true)
			bombsFound++;
		if (board[i + 1][j].IsBomb == true)
			bombsFound++;
		if (board[i + 1][j + 1].IsBomb == true)
			bombsFound++;
	}
	// option 9
	else if (i > 0 && j > 0)
	{
		if (board[i - 1][j - 1].IsBomb == true)
			bombsFound++;
		if (board[i - 1][j].IsBomb == true)
			bombsFound++;
		if (board[i - 1][j + 1].IsBomb == true)
			bombsFound++;
		if (board[i][j - 1].IsBomb == true)
			bombsFound++;
		if (board[i][j + 1].IsBomb == true)
			bombsFound++;
		if (board[i + 1][j - 1].IsBomb == true)
			bombsFound++;
		if (board[i + 1][j].IsBomb == true)
			bombsFound++;
		if (board[i + 1][j + 1].IsBomb == true)
			bombsFound++;
	}

	// after we checked every condition possible, we asks for the value we found;

	return bombsFound;
}
// initialzing the game board to it's final values; 
void init_board(Square* board, int rows, int cols)
{
	setBoardVisibleStateToHide(board, rows, cols);
	setBombsOnBoardRandomally(board, rows, cols);
	setCloseBombsValue(board, rows, cols);
}
// print's the game for the user to play; 
void printBoard(Square board[][MAX_SIZE], int rows, int cols, int countOf_)
{
	int colNum = 0;
	int rowNum = 0;
	printf("         ");
	for (int col = 0; col < cols; col++)
		if (col >= 0 && col < cols)
			if (col >= 0 && col < 9)
			{
				printf("%d   ", colNum);
				colNum++;
			}
			else {
				printf("%d  ", colNum);
				colNum++;
			}
	printf("\n");
	printf("    ");
	printLine(countOf_, '_');
	for (int row = 0; row < rows; row++)
	{
		if (row >= 0 && row < rows)
		{
			printf("    %2d |", rowNum);
			rowNum++;
		}
		for (int col = 0; col < cols; col++) {

			if (isInBoard(row, col, rows, cols))
			{

				if ((board[row][col].IsBomb == true) && (board[row][col].visibleState == 0)) {
					printf("%s", " X |");
				}
				if ((board[row][col].IsBomb == true) && (board[row][col].visibleState == 1))
				{
					printf("%s", " * |");
				}
				if (board[row][col].visibleState == 2)
				{
					printf("%s", " F |");
				}
				if ((board[row][col].visibleState == 0) && board[row][col].IsBomb == false)
				{
					printf("%s", " X |");
				}
				if ((board[row][col].visibleState == 1) && board[row][col].IsBomb == false)
				{

					if (board[row][col].BombsClose == 0)
					{
						printf("%s", "   |");
					}
					else {
						printf(" %d |", board[row][col].BombsClose);
					}

				}
			}

		}
		printf("\n");
	}
	printf("    ");
	printLine(countOf_, '_');
}
// print's the line for the next move of the player;
void printNextMove()
{
	printf("\nPlease enter your move, row and column : ");
}
// this function reveals the entire board if the player hited a bomb;
void uncoverBoard(Square board[][MAX_SIZE], int rows, int cols)
{
	for (int k = 0; k < rows; k++)
		for (int l = 0; l < cols; l++)
			board[k][l].visibleState = 1;
}
// this is a recursive functions that uncovers a square in the matrix until all the empty squares are sorounded by numeral squares;
int openSquare(Square board[][MAX_SIZE], int row, int col, int rows, int cols, int SquaresOpened)
{
	for (int i = -1; i <= 1; i++)
		for (int j = -1; j <= 1; j++)
			if (isInBoard(row + i, col + j, rows, cols) && board[row + i][col + j].visibleState == 0)
				if (board[row][col].BombsClose == 0 && board[row + i][col + j].visibleState == 0)
				{
					board[row + i][col + j].visibleState = 1;
					SquaresOpened++;
					SquaresOpened = openSquaresAround(board, row, col, rows, cols, SquaresOpened);

					for (int i = -1; i <= 1; i++)
						for (int j = -1; j <= 1; j++)
							if (isInBoard(row + i, col + j, rows, cols))
								SquaresOpened = openSquare(board, row + i, col + j, rows, cols, SquaresOpened);
				}
	return SquaresOpened;
}
// this function helps the function above do her job : )
int openSquaresAround(Square board[][MAX_SIZE], int row, int col, int rows, int cols, int SquaresOpened)
{
	for (int i = -1; i <= 1; i++)
		for (int j = -1; j <= 1; j++)
			if (isInBoard(row + i, col + j, rows, cols) && board[row + i][col + j].IsBomb == false && board[row + i][col + j].visibleState == 0)
			{
				board[row + i][col + j].visibleState = 1;
				SquaresOpened++;
			}
	return SquaresOpened;
}
// this function give us true or false if the square that we want to get access to is in our matrix;
bool isInBoard(int row, int col, int rows, int cols)
{
	return ((0 <= row) && (row < rows) && (0 <= col) && (col < cols));
}
// this function runs the procedure of the game according to the game rules;
Result game_procedure(Square board[][MAX_SIZE], int row, int col, char inputType, int num_of_rows, int num_of_cols, int size_of_, int SquaresOpened, int num_Of_Bombs, bool gameOver)
{
	Result result;

	if (inputType == 'O' || inputType == 'o')
	{
		if (isInBoard(row, col, num_of_rows, num_of_cols)) // to make sure user inserted valid input;
		{
			if (board[row][col].IsBomb == true) // check if bomb was hited, if so the board will be revealed and the game will be over with a loss messege;
			{
				uncoverBoard(board, num_of_rows, num_of_cols);
				result.gameOver = true;
				printBoard(board, num_of_rows, num_of_cols, size_of_);
				return result;
			}
			else if (board[row][col].visibleState == 1) // in case of inserting the same square on board;
				printf("Invalid move, please enter valid choice!\n\n");
			else if (board[row][col].BombsClose == 0) // when valid input has come the program reveals the square according to the input she read;
			{
				if (board[row][col].visibleState == 2)
				{
					board[row][col].visibleState = 1;
					SquaresOpened++;
				}
				SquaresOpened = openSquare(board, row, col, num_of_rows, num_of_cols, SquaresOpened); // to count how many squares were opened so far
			}
			else if (board[row][col].BombsClose > 0)
			{
				board[row][col].visibleState = 1;
				SquaresOpened++;
			}
		}
	}

	else if (inputType == 'F' || inputType == 'f')
	{
		if (isInBoard(row, col, num_of_rows, num_of_cols))
		{
			board[row][col].visibleState = 2;
		}
	}


	result.gameOver = gameOver;
	result.SquaresOpened = SquaresOpened;
	return result;
}
// this function open X cells accordding to the input from the user;
Result openNextXCells(Square board[][MAX_SIZE], int col, int num_of_rows, int num_of_cols, int SquaresOpened, int num_Of_Bombs, bool gameOver)
{
	Result result;
	int SquaresOpenedTmp = SquaresOpened;
	while (SquaresOpened <= col + SquaresOpenedTmp) // this is to get the "col" amount of iterations i need; 
	{
		
		if (SquaresOpened >= (num_of_rows * num_of_cols) - num_Of_Bombs)
		{
			result.gameOver = true;
			result.SquaresOpened = SquaresOpened;
			return result;
		}
		else if (SquaresOpened == col + SquaresOpenedTmp)
			break;
		
		for (int k = 0; k < num_of_rows; k++) // loop  that runs throuh the entire board;
		{
			if (col == 0 || SquaresOpened >= (num_of_rows * num_of_cols) - num_Of_Bombs || SquaresOpened >= col + SquaresOpenedTmp)
				break;
			for (int f = 0; f < num_of_cols; f++) // if we meet a square that is uncovered we make it reveal before us and we go on;
				if (board[k][f].IsBomb == false && board[k][f].visibleState == 0)
				{
					board[k][f].visibleState = 1;
					SquaresOpened++; // to keep in count how many we opened so far;
					if (SquaresOpened == col + SquaresOpenedTmp)
						break;
				}

				else if (board[k][f].IsBomb == false && board[k][f].visibleState == 1) // to skip if the specific square is already opened;
				{
					continue;
				}

		}
	}
	result.gameOver = false;
	result.SquaresOpened = SquaresOpened;
	return result;
}
// this function prints the menu and the size options and return a variable that contains the size of the board that was chosen; 
Choice printMenu(Square board[][MAX_SIZE], int rows, int cols)
{
	Choice sizeOf;
	int choice, checkInput;
	bool exit = false;

	while (!exit) // while loop in case of invalid choice;
	{
		printWelcome(); // print size menu;
		checkInput = scanf("%d", &choice);

		if (choice > 4 || choice < 0)
		{
			printf("\nPlease enter a valid choice!\n\n");
			continue;
		}
		else
		{
			break;
		}
	}
	switch (choice)
	{
	case 1:
		sizeOf.rows = 8;
		sizeOf.cols = 8;
		sizeOf.size_of_ = 36;
		break;
	case 2:
		sizeOf.rows = 12;
		sizeOf.cols = 12;
		sizeOf.size_of_ = 52;
		break;
	case 3:
		sizeOf.rows = 15;
		sizeOf.cols = 15;
		sizeOf.size_of_ = 64;
		break;
	case 4:
		printf("Please enter the size of the board (Lines <= 22 and Cols <= 22  ):");
		checkInput = scanf("%d%d", &sizeOf.rows, &sizeOf.cols);
		while (sizeOf.rows > 22 || sizeOf.rows < 0 || sizeOf.cols < 0 || sizeOf.cols > 22)
		{
			printf("Out Of Range , maximum Size is: 22): \n");
			printf("Please enter the size of the board (Lines <= 22 and Cols <= 22  ):");
			checkInput = scanf("%d %d", &sizeOf.rows, &sizeOf.cols);
		}
		sizeOf.size_of_ = 4 + (4 * sizeOf.cols);
		break;
	case 0:
		sizeOf.rows = 0;
		break;
	}

	return sizeOf;
}