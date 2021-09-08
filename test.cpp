#include <iostream>
#include <ctype.h>
#define BOARD_LENGTH 9
#define BOARD_SIZE 3

using namespace std;

bool isValidPlayLocation(int location);
void printBoard(void);
void requestUserInput(void);
void playInLocation(int, char);
// void initializeBoard(char board[size][size], int size);
// void printBoard(int location);
char board[BOARD_SIZE][BOARD_SIZE] = {{'.', '1', '2'}, {'3', '4', '5'}, {'6', '7', '8'}};

int main()
{
  const char playTag = 'O';
  int location;

  printf("waiting for named pipes open ... \n");

  printf("named pipes opened and ready\n");

  // player 1: engage first
  while (true)
  {
    printf("Where do you want to play? Select between cells 0 through 8 (Otherwise to quit): ");
    cin >> location;

    if (!cin) // or if(cin.fail())
    {
      cin.clear();
      cout << "You entered a non-digit character." << endl;
      break;
    }
    if (!isValidPlayLocation(location))
      break;

    playInLocation(location, playTag);
    cout << "You have played in " << location << "cell" << endl;
    printBoard();
  }

  return 0;
}

bool isValidPlayLocation(int location)
{
  return (isdigit(location) && (location < 0 || location > 8)) ? false : true;
}

void printBoard(void)
{
  cout << "+-+-+-+" << endl;
  for (int row = 0; row < BOARD_SIZE; row++)
  {
    for (int col = 0; col < BOARD_SIZE; col++)
    {
      cout << "|" << board[row][col];
      if ((col + 1) % 3 == 0)
      {
        cout << "|" << endl;
        cout << "+-+-+-+" << endl;
      }
    }
  }
}

/* request user input */
void playInLocation(int loc, char tag)
{
  int row = (int)loc / BOARD_SIZE;
  int col = (int)loc % BOARD_SIZE;

  board[row][col] = tag;
}
