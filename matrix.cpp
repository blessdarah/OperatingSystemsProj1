#include <iostream>
#include <vector>
#include <stdlib.h>
#include <ctime>

using namespace std;

void printGameBoard(vector<vector<int>> &boardVector);



class GameBoard {
    protected:
        vector<vector<char>> board;

    public:
         GameBoard() { 
            this->reset();
         }

         void reset() {
             this->board ={ {'.', '1', '2'}, 
                           {'3', '4', '5'}, 
                           {'6', '7', '8'} }; 
         }

         void print() {

            vector<vector<char>>::const_iterator row;
            vector<char>::const_iterator value;

            cout << "+-+-+-+" << endl;
            for(row = this->board.begin(); row != this->board.end(); ++row) {
                for(value = row->begin(); value != row->end(); ++value) {
                    cout << "|" << *value; 
                }
                cout << "|" << endl << "+-+-+-+" << endl;
            }
         }

         void init() {
             this->print();
         }


        bool playInCell(int cellNumber, char playerTag)
        {
            // check if board is full 
                // else get cell
            if(this->playInPosition(cellNumber, playerTag)) {
                cout << "Played " << playerTag << " in " << cellNumber << endl;
                return true;
            }else {
                cout << "invalid position" << endl;
                return false;
            }
        }

        bool isValidPlayPosition(int playPosition){
            return (playPosition == 'O' || playPosition == 'X') ? false : true;
        }

        bool playInPosition(int cellNumber, char tag)
        {
            int row = (int)cellNumber / this->board.size();
            int col = (int)cellNumber % this->board.size();
            int currentCellEntry = this->board[row][col];

            if(this->isValidPlayPosition(currentCellEntry)) {
                this->board[row][col] = tag; 
                return true;
            }
            return false;
        }

        bool isBoardFull()
        {
            vector<vector<char>>::const_iterator row;
            vector<char>::const_iterator value;

            for(row = this->board.begin(); row != this->board.end(); ++row) {
                for(value = row->begin(); value != row->end(); ++value) {
                   if(*value != 'X' && *value != 'O') {
                       return false;
                   }
               }
            }
            return true;
        }
        
        void checkWinner()
        {
            vector<char> row1;
            vector<char> row2;
            vector<char> row3;
            
            vector<vector<char>>::const_iterator row;
            vector<char>::const_iterator value;

            int counter = 0;

            for(row = this->board.begin(); row != this->board.end(); ++row) {
                counter++;
                for(value = row->begin(); value != row->end(); ++value) {
                    
                    if(counter == 1) {
                        row1 = vector(row->begin(), row->end());
                        this->printVector(row1);
                        this->vectorHasSameValues(row1);
                    }
                    if(counter == 2) {
                        row2 = vector(row->begin(), row->end());
                        this->printVector(row2);
                        this->vectorHasSameValues(row2);
                    }
                    if(counter == 3) {
                        row3 = vector(row->begin(), row->end());
                        this->printVector(row3);
                        this->vectorHasSameValues(row3);

                        // check to left to bottom right diagonal
                       
                        vector<char> leftDiagonal = {{row1[0][0], row2[1][1], row3[2][2]}};
                        this->vectorHasSameValues(leftDiagonal);

                        // check to right to bottom left diagonal
                        vector<char> rightDiagonal = {{row1[0][2], row2[1][1], row3[2][0]}};
                        this->vectorHasSameValues(rightDiagonal);
                    }
               }
            }
        }

        void vectorHasSameValues(vector<char> &v){
            if ( std::adjacent_find( v.begin(),v.end(), std::not_equal_to<>() ) == v.end() )
            {
                std::cout << "All elements are equal each other" << std::endl;
            }else {
                cout << "Elements are not equal" << endl;
            }
        }

        void printVector(vector<char> &v)
        {
            for(auto value: v) {
                cout << value << " ";
            }
            cout << endl;
        }
};

class Player{
    protected:
        char tag;
        int points;
    public:
        Player(char _tag){
            this->points = 0;
            this->tag = _tag;
        }
        
        void info()
        {
            cout << "scores: " << this->points << endl;
        }

        void play(GameBoard *board, int cell)
        {
            cout << "Location: " << cell << endl;
            if(!board->isBoardFull()) {
                bool playResult = board->playInCell(cell, this->tag);
            }else {
                // check for wining tag
                board->checkWinner();
                // get player scores
                // declare winners and quit game
            }
            
            cout << "checking winner\n\n\n"; 
            board->checkWinner();

            // get player scores
            board->print();
        }
};


// class Game {
//     protected:
//         GameBoard &board;
//         Player &player1;
//         Player &player2;
//     public:
//         Game(GameBoard &b, Player &p1, Player &p2)
//         {
//             this->board = b;
//             this->player1 = p1;
//             this->player2 = p2;
//             cout << "Game started" << endl;
//         }
// 
// };



int main(void) {
     srand(time(0));
    GameBoard *board = new GameBoard();
    Player *p1 = new Player('X');
    Player *p2 = new Player('O');

    int counter = 0;
    while (counter < 21) {
     int rNumber = (rand() % 8) + 0;   
     if(counter % 2 == 0) {
         p1->play(board, rNumber);
     }else {
         p2->play(board, rNumber);
     }
     counter++;
    }
    // Game *game = new Game(board, p1, p2);
}


