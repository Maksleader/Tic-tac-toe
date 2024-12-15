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
	Cell** board = nullptr;
	Player player1, player2;
	GameState gameState = GameState::InProgress;
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
	std::cout << "Please enter your name: ";
	std::cin >> game.player1.playerName;
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

void initGame(Game& game)
{
	std::cout << "Please enter size of playground: ";
	std::cin >> game.Size;

	game.board = new Cell * [game.Size];

	for (int i = 0; i < game.Size; i++)
	{
		game.board[i] = new Cell[game.Size];

		for (int j = 0; j < game.Size; j++)
		{
			game.board[i][j] = Cell::Empty;
		}
	}

	initPlayers(game);
}

void deInitGame(Game& game)
{
	for (int i = 0; i < game.Size; i++)
	{
		delete[] game.board[i];
	}

	delete[] game.board;
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
			std::cout << " " << static_cast<char>(game.board[y][x]) << " " << "|";
		}

		std::cout << std::endl;
	}
}

Coord playerMove(const Game& game) {
	Coord move;
	do {
		std::cout << "Enter your move (y x): ";
		std::cin >> move.y >> move.x;
		move.x--;
		move.y--;
		if ((move.x >= 0 && move.x < game.Size) && (move.y >= 0 && move.y < game.Size) &&
			(game.board[move.y][move.x] == Cell::Empty))
		{
			break;
		}
		else {
			std::cout << "Invalid move. " << game.Size << "." << std::endl;
		}
	} while (true);

	return move;
}

bool checkHorizontalWinCondition(const Game& game, size_t xCord)
{
	for (int i = 0; i < game.Size - 1; i++)
	{
		if (game.board[i][xCord] == Cell::Empty)
		{
			return false;
		}
		else if (game.board[i][xCord] != game.board[i + 1][xCord])
		{
			return false;
		}
	}

	return true;
}

bool checkVerticalWinCondition(const Game& game, size_t yCord)
{
	for (int i = 0; i < game.Size - 1; i++)
	{
		if (game.board[yCord][i] == Cell::Empty)
		{
			return false;
		}
		else if (game.board[yCord][i] != game.board[yCord][i + 1])
		{
			return false;
		}
	}

	return true;
}

bool checkDiagonalWinCondition(const Game& game)
{
	bool mainDiagonalWin = true;
	bool antiDiagonalWin = true;

	// Check main diagonal
	for (int i = 0; i < game.Size - 1; i++) {
		if (game.board[i][i] == Cell::Empty || game.board[i][i] != game.board[i + 1][i + 1]) {
			mainDiagonalWin = false;
			break; // Exit loop early if main diagonal check fails
		}
	}

	// Check anti-diagonal
	for (int i = 0; i < game.Size - 1; i++) {
		if (game.board[i][game.Size - i - 1] == Cell::Empty ||
			game.board[i][game.Size - i - 1] != game.board[i + 1][game.Size - i - 2]) {
			antiDiagonalWin = false;
			break; // Exit loop early if anti-diagonal check fails
		}
	}

	// Return true if either diagonal wins
	return mainDiagonalWin || antiDiagonalWin;
}


GameState isWon(const Game& game) {
    // Check horizontal and vertical win conditions
    for (int i = 0; i < game.Size; i++) {
        if (checkHorizontalWinCondition(game, i) || checkVerticalWinCondition(game, i)) {
            return GameState::Win;
        }
    }

    // Check diagonal win conditions
    if (checkDiagonalWinCondition(game)) {
        return GameState::Win;
    }

	int openSpots = 0;
	for (size_t i = 0; i < game.Size; i++) {
		for (size_t j = 0; j < game.Size; j++) {
			if (game.board[i][j] == Cell::Empty) {
				openSpots++;
			}
		}
	}

	if (openSpots == 0)
	{
		return GameState::Draw;
	}

    return GameState::InProgress;
}

float minimax(Game& game, bool isMaximizing) {
	GameState state = isWon(game);

	// Base cases
	if (state == GameState::Win) {
		return isMaximizing ? -1 : 1;
	}
	if (state == GameState::Draw) {
		return 0;
	}

	if (isMaximizing) {
		float bestScore = - INFINITY;
		for (size_t i = 0; i < game.Size; i++) {
			for (size_t j = 0; j < game.Size; j++) {
				// Is the spot available?
				if (game.board[i][j] == Cell::Empty) {
					game.board[i][j] = game.player2.playerSymbol;
					float score = minimax(game, false);
					game.board[i][j] = Cell::Empty;
					bestScore = std::max(score, bestScore);
				}
			}
		}
		return bestScore;
	}
	else {
		float bestScore = INFINITY;
		for (size_t i = 0; i < game.Size; i++) {
			for (size_t j = 0; j < game.Size; j++) {
				// Is the spot available?
				if (game.board[i][j] == Cell::Empty) {
					game.board[i][j] = game.player1.playerSymbol;
					float score = minimax(game, true);
					game.board[i][j] = Cell::Empty;
					bestScore = std::min(score, bestScore);
				}
			}
		}
		return bestScore;
	}

}

Coord aiMove(Game& game) {
	float bestScore = -INFINITY;
	Coord bestMove;

	for (size_t y = 0; y < game.Size; y++) {
		for (size_t x = 0; x < game.Size; x++) {
			if (game.board[y][x] == Cell::Empty) {
				game.board[y][x] = game.player2.playerSymbol;
				float score = minimax(game, false);
				game.board[y][x] = Cell::Empty;

				if (score > bestScore) {
					bestScore = score;
					bestMove = { x, y };
				}
			}
		}
	}

	return bestMove;
}


void move(Game& game)
{
	std::string playerName = game.player1.turn ? game.player1.playerName : game.player2.playerName;
	std::cout << "Turn: " << playerName << std::endl;

	if (game.player1.turn)
	{
		Coord coordinate = playerMove(game);
		game.board[coordinate.y][coordinate.x] = game.player1.playerSymbol;
		game.player1.turn = false;
		game.player2.turn = true;
	}
	else
	{
		Coord coordinate = aiMove(game);
		game.board[coordinate.y][coordinate.x] = game.player2.playerSymbol;
		game.player2.turn = false;
		game.player1.turn = true;
	}
}

void clearSecreen()
{
	system("cls");
}


void getGameResult(Game& game)
{
	game.gameState = isWon(game);

	if (game.gameState == GameState::Win)
	{
		if (game.player1.turn == true)
		{
			std::cout << game.player2.playerName << " " << "is won";
		}
		else
		{
			std::cout << game.player1.playerName << " " << "is won";
		}
	}
	else if (game.gameState == GameState::Draw)
	{
		std::cout << "The game ended in a draw";
	}

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
		getGameResult(game);

	} while (game.gameState == GameState::InProgress);

	deInitGame(game);
}