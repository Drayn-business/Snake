#include <iostream>
#include <vector>
#include <raylib.h>
#include <random>

constexpr uint16_t WINDOW_WIDTH = 900;
constexpr uint16_t WINDOW_HEIGHT = 900;
constexpr uint8_t GRID_SIZE = 25;
constexpr uint8_t TILE_WIDTH = WINDOW_WIDTH / GRID_SIZE;
constexpr uint8_t TILE_HEIGHT = WINDOW_HEIGHT / GRID_SIZE;

constexpr Color BACKGROUND_COLOR = CLITERAL(Color) { 220, 220, 220, 255 };
constexpr Color SNAKE_COLOR = CLITERAL(Color) { 0, 200, 50, 255 };
constexpr Color APPLE_COLOR = CLITERAL(Color) { 200, 0, 0, 255 };

typedef enum {
	North,
	East,
	South,
	West
} Direction;

typedef struct {
	int posX;
	int posY;
	Direction direction;
} SnakePart;

typedef struct {
	int posX;
	int posY;
} Apple;

void ResetBoard(bool board[][GRID_SIZE], std::vector<SnakePart>* snake, Apple* apple) {
	for (int i = 0; i < GRID_SIZE; i++)
	{
		for (int j = 0; j < GRID_SIZE; j++)
		{
			board[i][j] = false;
		}
	}

	*snake = std::vector<SnakePart>();
	(*snake).push_back(SnakePart{ GRID_SIZE / 2, GRID_SIZE / 2, Direction::South });
	board[(*snake)[0].posX][(*snake)[0].posY] = true;
	(*snake).push_back(SnakePart{ GRID_SIZE / 2, GRID_SIZE / 2 - 1, Direction::South });
	board[(*snake)[1].posX][(*snake)[1].posY] = true;

	*apple = SpawnApple(board);
	board[(*apple).posX][(*apple).posY] = true;
}

void DrawGrid(int width, int height, int gridSize, Color color) {
	const int widthIncrement = width / gridSize;
	const int heightIncrement = height / gridSize;

	for (int i = 0; i <= gridSize; i++) {
		DrawLine(0, heightIncrement * i, width, heightIncrement * i, color);
		DrawLine(widthIncrement * i, 0, widthIncrement * i, height, color);
	}
}

Apple SpawnApple(bool board[][GRID_SIZE]) {
	int maxValue = GRID_SIZE;
	Apple newApple = Apple{ maxValue, 0 };

	while (newApple.posX == maxValue)
	{
		int newXPos = std::rand() % maxValue;
		int newYPos = std::rand() % maxValue;

		if (board[newXPos][newYPos] == false) {
			newApple = Apple{ newXPos, newYPos };
		}
	}

	return newApple;
}

Vector2 MapDirection(Direction direction) {
	switch (direction)
	{
	case Direction::North:
		return Vector2{ 0, -1 };

	case Direction::East:
		return Vector2{ 1, 0 };

	case Direction::South:
		return Vector2{ 0, 1 };

	case Direction::West:
		return Vector2{ -1, 0 };

	default:
		return Vector2{ 0, 0 };
	}
}

bool IsPartCollidingWithSnake(SnakePart currentPart, std::vector<SnakePart> snake) {
	int counter = 0;
	for (SnakePart part : snake) {
		if (part.posX == currentPart.posX && part.posY == currentPart.posY)
			counter++;
	}

	return counter > 1 ? true : false;
}

int main(void) {
	int frameCounter = 0;
	bool isReset = false;

	bool board[GRID_SIZE][GRID_SIZE] = { false };
	std::vector<SnakePart> snake = { SnakePart {GRID_SIZE / 2, GRID_SIZE / 2, Direction::South}, SnakePart {GRID_SIZE / 2, GRID_SIZE / 2 - 1, Direction::South} };
	board[GRID_SIZE / 2][GRID_SIZE / 2] = true;
	board[GRID_SIZE / 2][GRID_SIZE / 2 - 1] = true;

	Apple apple = SpawnApple(board);

	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Snake");

	SetTargetFPS(60);

	while (!WindowShouldClose())
	{
		frameCounter += 1;
		
		if (IsKeyPressed(KEY_W) && snake[0].direction != Direction::South) snake[0].direction = Direction::North;
		if (IsKeyPressed(KEY_D) && snake[0].direction != Direction::West) snake[0].direction = Direction::East;
		if (IsKeyPressed(KEY_S) && snake[0].direction != Direction::North) snake[0].direction = Direction::South;
		if (IsKeyPressed(KEY_A) && snake[0].direction != Direction::East) snake[0].direction = Direction::West;
		if (IsKeyPressed(KEY_R)) isReset = true;

		if (isReset) {
			frameCounter = 0;
			ResetBoard(board, &snake, &apple);
			isReset = false;
		}

		if (frameCounter % 20 == 0) {
			for (int i = 0; i < snake.size(); i++)
			{
				SnakePart& part = snake[i];
				board[part.posX][part.posY] = false;

				Vector2 directionVector = MapDirection(part.direction);
				part.posX += (int)directionVector.x;
				part.posY += (int)directionVector.y;
				
				if (part.posX < 0 || part.posX >= GRID_SIZE ||
					part.posY < 0 || part.posY >= GRID_SIZE) {
					isReset = true;
				}

				if (IsPartCollidingWithSnake(part, snake)) {
					isReset = true;
				}

				board[part.posX][part.posY] = true;
			}

			for (int i = (int)(snake.size()) - 1; i > 0; i--) {
				snake[i].direction = snake[i - 1].direction;
			}

			if (snake[0].posX == apple.posX && snake[0].posY == apple.posY) {
				board[apple.posX][apple.posY] = false;
				apple = SpawnApple(board);

				SnakePart lastPart = snake[snake.size() - 1];
				Vector2 directionVector = MapDirection(lastPart.direction);
				snake.push_back(SnakePart{ lastPart.posX - (int)directionVector.x, lastPart.posY - (int)directionVector.y, lastPart.direction });

				board[apple.posX][apple.posY] = true;
			}
		}

		BeginDrawing();

		ClearBackground(BACKGROUND_COLOR);

		for (SnakePart part : snake) {
			DrawRectangle(part.posX * TILE_WIDTH, part.posY * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, SNAKE_COLOR);
		}

		DrawRectangle(apple.posX * TILE_WIDTH, apple.posY * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, APPLE_COLOR);

		DrawGrid(WINDOW_WIDTH, WINDOW_HEIGHT, GRID_SIZE, BLACK);

		EndDrawing();

		if (frameCounter == 60) {
			frameCounter = 0;
		}
	}
	CloseWindow();

	return EXIT_SUCCESS;
}