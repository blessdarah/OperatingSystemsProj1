#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <array>
#include <iostream>
#include "my_const.h"
#define BOARD_SIZE 3

using namespace std;

void showBoard(array<array<char, BOARD_SIZE>, BOARD_SIZE> &);
void playInLocation(int, array<array<char, BOARD_SIZE>, BOARD_SIZE> &, char);
bool game_has_winner(array<array<char, BOARD_SIZE>, BOARD_SIZE> &);
bool noPlaySpaceInBoard(array<array<char, BOARD_SIZE>, BOARD_SIZE> &);
bool isValidPlayLocation(int);
void resetGameBoard(array<array<char, BOARD_SIZE>, BOARD_SIZE> &);
bool has_same_values(array<char, BOARD_SIZE> &arr);
void show_game_stats(struct Payload &);

char playerTag = 'O';

struct Payload
{
	// struct Points points;
	int p1_score;
	int p2_score;
	array<array<char, BOARD_SIZE>, BOARD_SIZE> game_board;
	bool quit;
};
int main()
{
	// create the named pipe (FIFO) if not created yet
	int f1 = mkfifo(player1to2, 0666);
	int f2 = mkfifo(player2to1, 0666);
	printf("@p2: f1 = %d  f2 = %d\n", f1, f2);

	// main game_board payload to be shared
	array<array<char, BOARD_SIZE>, BOARD_SIZE> game_board = {{'.', '1', '2', '3', '4', '5', '6', '7', '8'}};
	struct Payload payload = {0, 0 ,game_board, false};

	char playerTag = 'X';
	int cell_location;

	printf("waiting for named pipes open ... \n");

	// P1&P2: order of open is important to unblock process
	// open() for RD will be blocked until the other side is open for WR
	int fd_rd = open(player1to2, O_RDONLY);

	// open() for WR will be blocked until the other side is open for RD
	int fd_wr = open(player2to1, O_WRONLY);

	printf("named pipes opened and ready\n");

	// prog2: read first
	while (true)
	{
		read(fd_rd, &payload, sizeof(payload));
		cout << "received game_board payload from player 1: " << endl;
		showBoard(payload.game_board);

		// check if there is a winning move from previous play
		if (game_has_winner(payload.game_board))
		{
			cout << "Winner: Player 1" << endl;
		}

		if (noPlaySpaceInBoard(payload.game_board))
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
			show_game_stats(payload); // show game stats
			break;
		}

		printf("Where do you want to play? Select between cells 0 through 8 (Otherwise to quit): ");
		cin >> cell_location;

		if (!cin) // check if user input does not matche an integer value
		{
			cin.clear();
			cout << "You entered a non-digit character." << endl;
			cout << "Quiting appliction" << endl;
			break;
		}
		else
		{
			if (isValidPlayLocation(cell_location))
			{
				playInLocation(cell_location, payload.game_board, playerTag);

				if (game_has_winner(payload.game_board))
				{
					payload.p2_score++; // if wining move, add points of player 2
					payload.quit = true; // change quit to true
					showBoard(payload.game_board);
					cout << "Winner: Player 2" << endl;
					show_game_stats(payload);
					write(fd_wr, &payload, sizeof(payload));
					break;
				}

				if (noPlaySpaceInBoard(payload.game_board))
				{
					payload.quit = true;
					show_game_stats(payload);
					write(fd_wr, &payload, sizeof(payload));
					break;
				}

				showBoard(payload.game_board);
				write(fd_wr, &payload, sizeof(payload));
			}
			else
			{
				cout << "Please select a valid cell_location from 0 to 8" << endl;
				printf("Where do you want to play? Select between cells 0 through 8 (Otherwise to quit): ");
				cin >> cell_location;
			}
		}
	}
	close(fd_rd);
	close(fd_wr);
}

bool isValidPlayLocation(int cell_location)
{
	return (cell_location < 0 || cell_location > 8) ? false : true;
}

void showBoard(array<array<char, BOARD_SIZE>, BOARD_SIZE> &game_board)
{
	cout << "+-+-+-+" << endl;
	for (auto const &row : game_board)
	{
		for (auto const &entry : row)
		{
			cout << "|" << entry;
		}
		cout << "|" << endl
				 << "+-+-+-+" << endl;
	}
}

void playInLocation(int loc, array<array<char, BOARD_SIZE>, BOARD_SIZE> &game_board, char tag)
{
	int row = (int)loc / BOARD_SIZE;
	int col = (int)loc % BOARD_SIZE;

	if (game_board[row][col] == 'O' || game_board[row][col] == 'X')
	{
		cout << "That cell_location is used already. Try something else" << endl;
		return;
	}
	game_board[row][col] = tag;
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
void show_game_stats(Payload &payload)
{
	cout << "================= Game Stats ================" << endl;
	cout << "Player one points: " << payload.p1_score << endl;
	cout << "Player two points: " << payload.p2_score << endl;
	printf("==============================================\n\n");
}

bool noPlaySpaceInBoard(array<array<char, BOARD_SIZE>, BOARD_SIZE> &game_board)
{
	for (auto const &row : game_board)
	{
		for (auto const &entry : row)
		{
			if (entry != 'O' && entry != 'X')
				return false;
		}
	}
	return true;
}

void resetGameBoard(array<array<char, BOARD_SIZE>, BOARD_SIZE> &game_board)
{
	game_board = {{'.', '1', '2', '3', '4', '5', '6', '7', '8'}};
}



