#include <unistd.h>		 // open() close()
#include <fcntl.h>		 // O_* constants
#include <sys/stat.h>	 // mkfifo()
#include <sys/types.h> // mkfifo()
#include <string.h>		 // strlen() strcmp()
#include <stdio.h>		 // printf() scanf()
#include <stdlib.h>		 // exit()
#include <iostream>
#include <array>
#include "my_const.h"

#define BOARD_SIZE 3

using namespace std;

bool isValidPlayLocation(int);
void showBoard(array<array<char, BOARD_SIZE>, BOARD_SIZE> &);
void playInLocation(int, array<array<char, BOARD_SIZE>, BOARD_SIZE> &, char);
bool game_has_winner(array<array<char, BOARD_SIZE>, BOARD_SIZE> &);
bool noPlaySpaceInBoard(array<array<char, BOARD_SIZE>, BOARD_SIZE> &);
bool has_same_values(array<char, BOARD_SIZE> &arr);
void resetGameBoard(array<array<char, BOARD_SIZE>, BOARD_SIZE> &);
void show_game_stats(struct Payload &);

char playerTag = 'X';

struct Points
{
	int p1 = 0;
	int p2 = 0;
};

struct Payload
{
	int p1_score;
	int p2_score;
	array<array<char, BOARD_SIZE>, BOARD_SIZE> board;
	bool quit;
};

int main()
{
	// create the named pipe (FIFO) if not exist
	int f1 = mkfifo(player1to2, 0666);
	int f2 = mkfifo(player2to1, 0666);
	printf("@p1: f1 = %d  f2 = %d\n", f1, f2);

	// main board payload to be shared
	array<array<char, BOARD_SIZE>, BOARD_SIZE> board = {{'.', '1', '2', '3', '4', '5', '6', '7', '8'}};
	struct Points points = {0, 0};
	struct Payload payload = {0, 0, board, false};

	char playTag = 'O';
	int location;

	printf("waiting for named pipes open... \n");

	// P1&P2: order of open() is important to unblock processes
	// open() for WR will be blocked until the other side is open for RD
	int fd_wr = open(player1to2, O_WRONLY);

	// open() for RD will be blocked until the other side is open for WR
	int fd_rd = open(player2to1, O_RDONLY);

	printf("named pipes opened and ready\n");

	// player 1: engage first
	showBoard(payload.board);
	while (true)
	{
		printf("Where do you want to play? Select between cells 0 through 8 (Otherwise to quit): ");
		cin >> location;

		if (!cin) // check if user input does not matche an integer value
		{
			cin.clear();
			cout << "You entered a non-digit character." << endl;
			cout << "Quiting appliction" << endl;
			break;
		}
		else
		{
			if (isValidPlayLocation(location))
			{
				playInLocation(location, payload.board, playTag);
				if (game_has_winner(payload.board))
				{
					payload.p1_score++; // add the points of p1
					payload.quit = true; // set quit to true
					showBoard(payload.board);
					cout << "Winner: Player 1" << endl;
					show_game_stats(payload);
					write(fd_wr, &payload, sizeof(payload));
					break;
				}

				if (noPlaySpaceInBoard(payload.board))
				{
					cout << "The game has ended in a tie." << endl;
					payload.quit = true;
					show_game_stats(payload);
					write(fd_wr, &payload, sizeof(payload));
					break;
				}
			}
			else
			{
				cout << "Please select a valid location from 0 to 8" << endl;
				printf("Where do you want to play? Select between cells 0 through 8 (Otherwise to quit): ");
				cin >> location;
			}
		}
		// writing to player 2
		showBoard(payload.board);
		write(fd_wr, &payload, sizeof(payload));

		// reading from player 2
		read(fd_rd, &payload, sizeof(payload));
		showBoard(payload.board);

		// check if there is a winning move from previous play
		if (game_has_winner(payload.board))
		{
			cout << "Winner: Player 2" << endl;
		}

		if (noPlaySpaceInBoard(payload.board))
		{
			cout << "The game has ended in a tie." << endl;
			payload.quit = true;
			show_game_stats(payload);
			write(fd_wr, &payload, sizeof(payload));
			break;
		}

		//check if it's time to quite the application
		if (payload.quit)
		{
			// show game stats
			show_game_stats(payload);
			break;
		}
	}
	close(fd_wr);
	close(fd_rd);
	unlink(player1to2);
	unlink(player2to1);
}

bool isValidPlayLocation(int location)
{
	return (location < 0 || location > 8) ? false : true;
}

void showBoard(array<array<char, BOARD_SIZE>, BOARD_SIZE> &board)
{
	cout << "+-+-+-+" << endl;
	for (auto const &row : board)
	{
		for (auto const &entry : row)
		{
			cout << "|" << entry;
		}
		cout << "|" << endl
				 << "+-+-+-+" << endl;
	}
}

void playInLocation(int loc, array<array<char, BOARD_SIZE>, BOARD_SIZE> &board, char tag)
{
	int row = (int)loc / BOARD_SIZE;
	int col = (int)loc % BOARD_SIZE;

	if (board[row][col] == 'O' || board[row][col] == 'X')
	{
		cout << "That location is used already. Try something else" << endl;
		return;
	}
	board[row][col] = tag;
	cout << "You have played in cell " << loc << endl;
}

bool has_same_values(array<char, BOARD_SIZE> &arr)
{
	char value = arr[0];
	for (int i = 0; i < arr.size(); i++)
	{
		if(arr[i] != value) return false;
	}
	return true;
}


// check validation criteria
bool game_has_winner(array<array<char, BOARD_SIZE>, BOARD_SIZE> &game_board)
{
	// check for rows
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		array<char, BOARD_SIZE> row = game_board[i];
		if(has_same_values(row))
		{
			cout << "rows match: " << true << endl;
			playerTag = row[0];
			return true;
		}
	}

	// leding diagonals
	array<char, BOARD_SIZE> left_leading_diagonal;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j <= i; j++) {
			if(i == j) left_leading_diagonal[i] = game_board[i][j];
		}
	}
	if(has_same_values(left_leading_diagonal))
	{
		playerTag = left_leading_diagonal[0];
		cout << "left diagonal match: " << true << endl;
		return true;
	}

	array<char, BOARD_SIZE> right_leading_diagonal;
	for (int i = BOARD_SIZE - 1; i >= 0; i--) {
		for (int j = 0; j <= i; j++) {
			if(i == j) right_leading_diagonal[i] = game_board[i][j];
		}
	}
	if(has_same_values(right_leading_diagonal)) 
	{
		playerTag = right_leading_diagonal[0];
		cout << "right diagonal match: " << true << endl;
		return true;
	}

	// check for columns
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		array<char, BOARD_SIZE> column;
		for(int j = 0; j < BOARD_SIZE; j++)
		{
			column[j] = game_board[j][i];
		}
		if(has_same_values(column))
		{
			playerTag = column[0];
			return true;
			cout << "columns match: " << true << endl;
		}
	}

	return false;
}

bool noPlaySpaceInBoard(array<array<char, BOARD_SIZE>, BOARD_SIZE> &board)
{
	for (auto const &row : board)
	{
		for (auto const &entry : row)
		{
			if (entry != 'O' && entry != 'X')
				return false;
		}
	}
	return true;
}

void resetGameBoard(array<array<char, BOARD_SIZE>, BOARD_SIZE> &board)
{
	board = {{'.', '1', '2', '3', '4', '5', '6', '7', '8'}};
}

void show_game_stats(Payload &payload)
{
	cout << "================= Game Stats ================" << endl;
	cout << "Player one points: " << payload.p1_score << endl;
	cout << "Player two points: " << payload.p2_score << endl;
	printf("==============================================\n\n");
}