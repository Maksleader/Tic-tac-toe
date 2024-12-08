#include <iostream>
#include <chrono>
#include <random>

enum class GameState {
	Win,
	Draw,
	InProgress
};

enum class Cell {
	Empty = '_',
	Zero = 'O',
	Cross = 'X'
};

struct Player
{
	bool turn = false;
	std::string playerName;
	Cell playerSymbol;
};

struct Coord
{
	size_t x = 0;
	size_t y = 0;
};

struct Game
{
	size_t Size = 3;
	Cell** playGround = nullptr;
	Player player1, player2;
	GameState gameState = GameState::InProgress;
	size_t numberOfMoves = 0;
	Coord* centralCoords;
};

int32_t getRandomNumber(int32_t min, int32_t max)
{
	const static auto seed = std::chrono::system_clock::now().time_since_epoch().count();
	static std::mt19937_64 generator(seed);
	std::uniform_int_distribution<int32_t> dis(min, max);

	return dis(generator);
}

void initPlayers(Game& game)
{
	//std::cout << "Please enter your name: ";
	//std::cin >> game.player1.playerName;
	game.player1.playerName = "Mahmud";
	game.player2.playerName = "AI";

	if (getRandomNumber(0, 1000) % 2 == 0)
	{
		game.player1.turn = true;
		game.player2.turn = false;
		game.player1.playerSymbol = Cell::Cross;
		game.player2.playerSymbol = Cell::Zero;
	}
	else
	{
		game.player2.turn = true;
		game.player1.turn = false;
		game.player2.playerSymbol = Cell::Cross;
		game.player1.playerSymbol = Cell::Zero;
	}
}


Coord* findCentralCoordsForEvenSizePlayGround(size_t size)
{
	const size_t centralCoordsCount = 4;
	Coord* coords = new Coord[centralCoordsCount];
	coords[0].x = size / 2;
	coords[0].y = size / 2;

	coords[1].x = size / 2 - 1;
	coords[1].y = size / 2 - 1;

	coords[2].x = size / 2 - 1;
	coords[2].y = size / 2;

	coords[3].x = size / 2;
	coords[3].y = size / 2 - 1;

	for (int i = 0; i< centralCoordsCount; i++)
	{
		std::cout << "cords[" << i << "]= " << coords[i].x << " " << coords[i].y <<std::endl;
	}

	return coords;
}


Coord getCentralCoord(const Game& game)
{
	Coord coord;

	if (game.Size % 2 == 0)
	{
		const size_t centralCoordsCount = 4;

		Coord* centralCoords = findCentralCoordsForEvenSizePlayGround(game.Size);

		std::vector<int> indices = { 0, 1, 2, 3 };
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::shuffle(indices.begin(), indices.end(), std::default_random_engine(seed));

		for (int index : indices)
		{
			size_t x = centralCoords[index].x;
			size_t y = centralCoords[index].y;

			if (game.playGround[y][x] == Cell::Empty)
			{
				coord = centralCoords[index];
				delete[] centralCoords;
				break;
			}
		}
	}
	else
	{
		coord.x = game.Size / 2;
		coord.y = game.Size / 2;
	}

	return coord;
}

void initGame(Game& game)
{
	std::cout << "Please enter size of playground: ";
	std::cin >> game.Size;

	game.playGround = new Cell * [game.Size];

	for (int i = 0; i < game.Size; i++)
	{
		game.playGround[i] = new Cell[game.Size];

		for (int j = 0; j < game.Size; j++)
		{
			game.playGround[i][j] = Cell::Empty;
		}
	}

	initPlayers(game);
}

void deInitGame(Game& game)
{
	for (int i = 0; i < game.Size; i++)
	{
		delete[] game.playGround[i];
	}

	delete[] game.playGround;
}

void draw(const Game& game)
{
	std::cout << "    ";
	for (int i = 0; i < game.Size; i++)
	{
		std::cout << i + 1 << "   ";
	}

	std::cout << std::endl;

	for (int y = 0; y < game.Size; y++)
	{
		std::cout << " " << y + 1 << "|";

		for (int x = 0; x < game.Size; x++)
		{
			std::cout << " " << static_cast<char>(game.playGround[y][x]) << " " << "|";
		}

		std::cout << std::endl;
	}

	std::string playerName = game.player1.turn ? game.player1.playerName : game.player2.playerName;

	std::cout << "Turn: " << playerName << std::endl;
}

Coord playerMove(const Game& game) {
	Coord move;
	do {
		std::cout << "Enter your move (y x): ";
		std::cin >> move.y >> move.x;
		move.x--;
		move.y--;
		if ((move.x >= 0 && move.x < game.Size) && (move.y >= 0 && move.y < game.Size) &&
			(game.playGround[move.y][move.x] == Cell::Empty))
		{
			break;
		}
		else {
			std::cout << "Invalid move. Please enter values greater than 0 and less than " << game.Size << "." << std::endl;
		}
	} while (true);

	return move;
}

Coord aiMove(const Game& game)
{
	Coord move;

	Coord centralElement = getCentralCoord(game);

	if (centralElement.x != 0 && centralElement.y != 0)
	{
		return  centralElement;
	}
	

}

void move(Game& game)
{
	if (game.player1.turn)
	{
		Coord coordinate = playerMove(game);
		game.playGround[coordinate.y][coordinate.x] = game.player1.playerSymbol;
		game.player1.turn = false;
		game.player2.turn = true;
	}
	else
	{
		Coord coordinate = aiMove(game);
		game.playGround[coordinate.y][coordinate.x] = game.player2.playerSymbol;
		game.player2.turn = false;
		game.player1.turn = true;
	}
}

void clearSecreen()
{
	system("cls");
}

int main()
{
	Game game;
	initGame(game);
	clearSecreen();
	draw(game);

	do
	{
		move(game);
		clearSecreen();
		draw(game);


	} while (game.gameState == GameState::InProgress);

	deInitGame(game);
}