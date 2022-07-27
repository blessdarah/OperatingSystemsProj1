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
void printGameBoard(vector<vector<int>> &);
void markPlayerEntry(int selectedPoint, vector<vector<int>> &);

bool isWinningMove(<vector<vector<int>> &);
bool isBoardFull(<vector<vector<int>> &);
void resetBoard(<vector<vector<int>> &);
void printStats(struct Points &);

char winingTag = 'X';
char playTag = 'O';

struct Points
{
	int p1 = 0;
	int p2 = 0;
};

struct Data
{
	struct Points points;
	vector<vector<int>> board;
	bool quit;
	bool pause;
};

int main()
{
	// create the named pipe (FIFO) if not exist
	int f1 = mkfifo(player1to2, 0666);
	int f2 = mkfifo(player2to1, 0666);

	printf("@p1: f1 = %d  f2 = %d\n", f1, f2);

	// main board data to be shared
    vector<vector<int>> board{ {0, 1, 2}, 
                           {3, 4, 5}, 
                           {6, 7, 8} }; 

	struct Points points = {p1 : 0, p2 : 0};
	struct Data data = {points : points, board : board, quit : false, pause : false};

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
	printGameBoard(data.board);

    return 0;
	while (true)
	{
		printf("Where do you want to play? Select between cells 0 through 8 (Otherwise to quit): ");
        try{
            cin >> location;
        }catch(int exception){
            cout << "Quitting app. You entered an non-digit value" << endl;
            throw exception;
        }

        if (isValidPlayLocation(location))
        {
            markPlayerEntry(location, data.board);
            if (isWinningMove(data.board))
            {
                data.points.p1++; // add the points of p1
                data.quit = true; // set quit to true
                printGameBoard(data.board);
                cout << "Winner: Player 1" << endl;
                printStats(data.points);
                write(fd_wr, &data, sizeof(data));
                break;
            }

            if (isBoardFull(data.board))
            {
                cout << "The game has ended in a tie." << endl;
                data.quit = true;
                printStats(data.points);
                write(fd_wr, &data, sizeof(data));
                break;
            }
        }
        else
        {
            cout << "Please select a valid location from 0 to 8" << endl;
            printf("Where do you want to play? Select between cells 0 through 8 (Otherwise to quit): ");
            cin >> location;
        }
		// writing to player 2
		printGameBoard(data.board);
		write(fd_wr, &data, sizeof(data));

		// reading from player 2
		read(fd_rd, &data, sizeof(data));
		printGameBoard(data.board);

		// check if there is a winning move from previous play
		if (isWinningMove(data.board))
		{
			cout << "Winner: Player 2" << endl;
		}

		if (isBoardFull(data.board))
		{
			cout << "The game has ended in a tie." << endl;
			data.quit = true;
			printStats(data.points);
			write(fd_wr, &data, sizeof(data));
			break;
		}

		//check if it's time to quite the application
		if (data.quit)
		{
			// show game stats
			printStats(data.points);
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

void printGameBoard(vector<vector<int>> &boardVector) {

    // row iterator
    vector<vector<int>>::const_iterator row;
    vector<int>::const_iterator value;

    cout << "+-+-+-+" << endl;
    for(row = boardVector.begin(); row != boardVector.end(); ++row) {
        for(value = row->begin(); value != row->end(); ++value) {
            if(*value == 0) {
                cout << "|.";
            }else {
                cout << "|" << *value; 
            }
        }
        cout << "|" << endl;
        cout << "+-+-+-+" << endl;
    }
}





void markPlayerEntry(int selectedPoint, vector<vector<int>> &boardRef)
{
	int row = (int)selectedPoint / boardRef.size();
	int col = (int)selectedPoint % boardRef.size();
    int currentCellEntry = boardRef[row][col];

    if(currentCellEntry == 'O' || currentCellEntry == 'X')
    {
		cout << "That location is used already. Try something else" << endl;
    }else{
        currentCellEntry = playTag;
        cout << "Played " << playTag << "at " << selectedPoint << endl;
    }
}

// check validation criteria
bool isWinningMove(<vector<vector<int>> &board)
{
	// check for rows
	for (int i = 0; i < board.size(); i++)
	{
		if (board[i][0] == board[i][1] && board[i][1] == board[i][2])
		{
			winingTag = board[i][0];
			return true;
		}
	}

	// check for columns
	for (int i = 0; i < board.size(); i++)
	{
		if (board[0][i] == board[1][i] && board[1][i] == board[2][i])
		{
			winingTag = board[0][i];
			return true;
		}
	}

	// check for diagonals
	if (board[0][0] == board[1][1] && board[1][1] == board[2][2])
	{
		winingTag = board[0][0];
		return true;
	}

	if (board[0][2] == board[1][1] && board[1][1] == board[2][0])
	{
		winingTag = board[0][2];
		return true;
	}

	return false;
}

bool isBoardFull(<vector<vector<int>> &board)
{
    vector<vector<int>>::const_iterator row;
    vector<int>::const_iterator value;

    for(row = boardVector.begin(); row != boardVector.end(); ++row) {
        for(value = row->begin(); value != row->end(); ++value) {
            if(value != 'O' && value != 'X'){
                return false;
            }
        }
    }
	return true;
}

void resetBoard(vector<vector<int>> &board)
{
	board = { {0, 1, 2}, 
           {3, 4, 5}, 
           {6, 7, 8} }; 
}

void printStats(Points &points)
{
	cout << "================= Game Stats ================" << endl;
	cout << "Player one points: " << points.p1 << endl;
	cout << "Player two points: " << points.p2 << endl;
	printf("==============================================\n\n");
}
