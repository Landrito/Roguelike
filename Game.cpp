#include <iostream>
#include <string.h>
#include <vector>
#include <stdio.h>
using namespace std;

//Board Sizes
const int BOARD_X_SIZE = 81; //this is size 81 to account for the null char at the end of stirngs
const int BOARD_Y_SIZE = 23;

enum direction {NO_DIRECTION, UP, DOWN, LEFT, RIGHT};

//Forward declaration so goblin knows about the game;
class game;

class goblin
{
public:
	//I will add these things soon
	goblin();
	goblin(const goblin & src);
	goblin& operator=(const goblin & src);
	~goblin();
	goblin(int x, int y);
	int goblinAI( const game & src );

//private: 
//I will turn this into a private data member soon, I didn't have enough time to finish it this time =/
	int goblinXPos;
	int goblinYPos;
};

class game
{
public:
	game();
	game(const game & src);
	~game();
	game& operator=(const game & src);

	bool loadLevel();
	bool canMove(const int & xDelta, const int & yDelta) const;
	bool movePlayer(char command);
	void moveGoblin(int direction, const goblin gob);
	bool isFree(int xPos, int yPos) const;
	void updateGoblins();
	void printBoard() const;
	bool isDone() const;

private:
	int playerXPos;
	int playerYPos;
	vector<goblin> goblins;
	char board[BOARD_Y_SIZE][BOARD_X_SIZE];
};


int main()
{
	//Declare the game
	game myGame;

	//Start the game if the level was loaded correctly
	if( myGame.loadLevel() )
	{
		//initialize the character to hold the command
		char command = '\n';
		myGame.printBoard();

		//Keep going until the player commands to exit the game
		while( command != 'x' )
		{
			std::cout << "Up(a) Left(a) Down(s) Right(d) Exit(x)    " 
					  << "Enter Your Command:";
			std::cin >> command;

			myGame.movePlayer(command);
			myGame.printBoard();

			myGame.updateGoblins();
		}
	}

	return 0;
}


//constructor to keep things safe
game::game() 
	: playerXPos(BOARD_X_SIZE / 2), playerYPos(BOARD_Y_SIZE / 2)
{
	//Initialize the board and make it able to work;
	for(int i = 0; i < BOARD_Y_SIZE; i++)
	{
		for(int j = 0; j < BOARD_X_SIZE; j++)
		{
			//Place a null char if we are at the end
			if(j == BOARD_X_SIZE - 1)
				this->board[i][j] = '\0';

			//Place a wall if we are at a border
			else if (i == 0 || i == BOARD_Y_SIZE - 1 || j == 0 || j == BOARD_X_SIZE - 2)
				this->board[i][j] = '#';

			//Place an empty spot if nothing special
			else
				this->board[i][j] = '.';
		}
	}
	//Place the player in his position as well
	board[playerYPos][playerXPos] = '@';
}

game::game(const game & src) 
	: playerXPos( src.playerXPos ), playerYPos( src.playerYPos )
{
	//copy each line in hte source board
	for(int i = 0; i < BOARD_Y_SIZE; i++)
		strcpy(this->board[i], src.board[i]);
}

game::~game() {}

game& game::operator=(const game & src)
{
	this->playerXPos = src.playerXPos;
	this->playerYPos = src.playerYPos;

	for(int i = 0; i < BOARD_Y_SIZE; i++)
		strcpy(this->board[i], src.board[i]);

	return *this;
}

bool game::canMove(const int & xDelta, const int & yDelta) const
{
	//This is just for safety, As long as the player starts within the boumds, it shouldn't work
	//The case where this will save our program would be if the programmer initializes a game and
	//something messes up and puts the player x and the player y
	if( playerXPos + xDelta >= BOARD_X_SIZE ||
		playerXPos + xDelta < 0 ||
		playerYPos + yDelta > BOARD_Y_SIZE ||
		playerYPos + yDelta < 0)
		return false;
	
	//The player can move if the space is open ('.')
	else
		return board[playerYPos + yDelta][playerXPos + xDelta] == '.';
}

bool game::movePlayer(char command)
{
	//initialize the deltas
	int xDelta = 0;
	int yDelta = 0; 

	//change the deltas based on the inputted command 
	if(command == 'w')
		yDelta = -1;

	else if(command == 'a')
		xDelta = -1;

	else if(command == 's')
		yDelta = 1;

	else if(command == 'd')
		xDelta = 1;

	if( canMove(xDelta, yDelta) )
	{
		//change where the player is to a period
		board[playerYPos][playerXPos] = '.';

		//change where the player is going to to the player
		board[playerYPos + yDelta][playerXPos + xDelta] = '@';

		//update the player position
		playerYPos = playerYPos + yDelta;
		playerXPos = playerXPos + xDelta;

		//return the the player moved
		return true;
	}

	return false;
}

void game::moveGoblin(int direction, const goblin gob)
{
		//initialize the deltas
	int xDelta = 0;
	int yDelta = 0; 

	//change the deltas based on the inputted command 
	if(direction == UP)
		yDelta = -1;

	else if(direction == LEFT)
		xDelta = -1;

	else if(direction == DOWN)
		yDelta = 1;

	else if(direction == RIGHT)
		xDelta = 1;

	if(direction != NO_DIRECTION)
	{
		board[gob.goblinYPos][gob.goblinXPos] = 'g';
		board[gob.goblinYPos - yDelta][gob.goblinXPos - xDelta] = '.';
	}
}

void game::updateGoblins()
{
	for( int i = 0; i < goblins.size(); i++)
	{
		moveGoblin( goblins[i].goblinAI(*this), goblins[i] );
	}

}

void game::printBoard() const
{
	//print an endline to make sure the board always looks good
	std::cout << std::endl;

	//print each line of the board 
	for(int i = 0; i < BOARD_Y_SIZE; i++)
		std::cout << board[i] <<  std::endl;		
}

bool game::isDone() const
{
	//The functionality of this can be changed later
	return false; 
}

//I put this at the bottom cause it's super distracting LOL
bool game::loadLevel()
{
	strcpy(board[0], "################################################################################");
	strcpy(board[1], "#g...........#..................................#............#.................#");
	strcpy(board[2], "#............#.#................................#............#.................#");
	strcpy(board[3], "#............#.#................................#............#.................#");
	strcpy(board[4], "#............#.#................................#............#.................#");
	strcpy(board[5], "#............#.#................................#.############.................#");
	strcpy(board[6], "#............#.#................................#.#............................#");
	strcpy(board[7], "#..............#................................#.#.##########.................#");
	strcpy(board[8], "################..................@.............#...#..........................#");
	strcpy(board[9], "#...............................................#####..........................#");
	strcpy(board[10], "#...............................................#..............................#");
	strcpy(board[11], "#.....................#########.................#..............................#");
	strcpy(board[12], "#.............................#.................#..............................#");
	strcpy(board[13], "#.............................#.................#####################..........#");
	strcpy(board[14], "#.............................#.....................................#..........#");
	strcpy(board[15], "#.....................##################............................#..........#");
	strcpy(board[16], "#.....................#................#............................#..........#");
	strcpy(board[17], "#.....................#................#............................#..........#");
	strcpy(board[18], "#.....................#................#............................#..........#");
	strcpy(board[19], "#.....................#................#.......................................#");
	strcpy(board[20], "#.....................#................#.......................................#");
	strcpy(board[21], "#.....................#........................................................#");
	strcpy(board[22], "################################################################################");

	playerXPos = 34;
	playerYPos = 8;

	goblins.push_back(goblin(1,1));
	return true;
}

int goblin::goblinAI( const game & src )
{
	int direction = rand() % 4 + 1;

	//initialize the deltas
	int xDelta = 0;
	int yDelta = 0; 

	//change the deltas based on the inputted command 
	if(direction == UP)
		yDelta = -1;

	else if(direction == LEFT)
		xDelta = -1;

	else if(direction == DOWN)
		yDelta = 1;

	else if(direction == RIGHT)
		xDelta = 1;

	if( src.isFree(this->goblinXPos + xDelta, this->goblinYPos + yDelta) )
	{
		this->goblinXPos += xDelta;
		this->goblinYPos += yDelta;
		return direction;
	}

	return NO_DIRECTION;
}

bool game::isFree(int xPos, int yPos) const
{
	return board[yPos][xPos] == '.';
}

goblin::goblin() : goblinXPos(1), goblinYPos(2) {}

goblin::goblin(const goblin & src) : goblinXPos(src.goblinXPos), goblinYPos(src.goblinYPos) {}

goblin::~goblin() {}

goblin::goblin(int x, int y) : goblinXPos(x), goblinYPos(y) {} 

goblin& goblin::operator=(const goblin & src)
{
	this->goblinXPos = src.goblinXPos;
	this->goblinYPos = src.goblinYPos;
	return *this;
}


