/* M: Alright, it appears to me that you are using char board[][] to store
      not just the terrain, but also the goblins and player, which is just asking
      for trouble. I'm guessing youd do this as you want to print directly from the char board[][]
      to the screen, but I don't think that is a good solution. See the discussion on FB
      for 2 possible implementations of the print function that will be better than this.
      
      Also something is wrong with your goblin movement code, but you probably already know this.
      When I run the program and a goblin moves, it does not remove the previous g of itself from the board.
      In addition to that the goblin can convert walls from '#' to '.'
      I'm guessing if you first fix the above issue, this will fix itself too.
      There is also no collision detection between the player and the goblin.
      Edit: Actually the player can't move on top of a goblin, but I feel there 
      should be more to it, such as the player dying as a result.
      
      Another note, you use literals like '.', '#', and '@' along with 'w' 'a', 's', 'd'
      that should probably be declared as constants.
*/

#include <iostream>
#include <string.h>
#include <vector>
#include <stdio.h>
#include <stdlib.h> //M: Wouldn't compile without this as it said it was missing rand()

using namespace std; //M: Bit of a nitpick, but it's bad practice to do this.

//Board Sizes
//M: I would avoid storing the board as a set of strings as it should
//   be a 2d array of chars, not a 1d array of char strings in the way we use it.
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
	
	//M: Do you need an assignment operator for goblins?
	//   It seems unnecessary as goblins should be unique and this would create a clone.
	//   Evan said not to add unnecessary code. If you have a good reason why, then nevermind.
	//   Same thing with the copy constructor.
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
	bool isDone() const; //M: I think isGameOver would be a better name, isDone implies that the regular state is not done.

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
	//M: If you go through the trouble of checking to see if the level
	//   loads correctly, then you should at least add an error message if it doesn't.
	if( myGame.loadLevel() )
	{
		//initialize the character to hold the command
		char command = '\n';
		myGame.printBoard();

		//Keep going until the player commands to exit the game
		while( command != 'x' )
		{
		    //M: Typo, Should be Up(w) instead of Up(a)
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

//M: Not sure when copying a game would be useful.
game::game(const game & src) 
	: playerXPos( src.playerXPos ), playerYPos( src.playerYPos )
{
	//copy each line in hte source board
	for(int i = 0; i < BOARD_Y_SIZE; i++)
		strcpy(this->board[i], src.board[i]);
}

game::~game() {}

//M: Again, not sure if you would ever want to copy a game.
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

	//M: I'm not sure if using an ENUM is appropriate here.
	//   What if you later change the ENUM, but you don't update the code here?
	//   The problem is that you are taking an int and treating it as an ENUM,
	//   which works in this case, but is probably a bad idea.
	//   Double check with Evan though, but it just feels wrong.

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


