#include <iostream>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
// A simple "snake" clone.  Or "worms.io" for the new kids
// Library from One Lone Coder
// Snake Game code by Kaltinril
// This is the non-OO version
// We will a struct for the X/Y position of each snake body part
// Other ways to do this would be to create a dynamic array, and each time the worm grows, add on to it
// Another way would be to create a list or linked list, or doubly-linked list
// All of those work, but add complexity and I want this first snake version to be as simple as possible


class SnakeGame : public olc::PixelGameEngine
{
public:
	SnakeGame()
	{
		sAppName = "Yummy!";
	}

private:
	// Create a simply X/Y structure for easier access
	struct point2D
	{
		int x;
		int y;
	};

	struct rgbColor
	{
		int red;
		int green;
		int blue;
	};

	struct fruitInstance
	{
		point2D point;
		rgbColor color;
	};

	// Snake variables
	int snakeLength = 3;  // Lets start the snake out with a head, a body, and a tail (length of 3)
	const static int maxSnakeLength = 1000;
	point2D snakeBody[maxSnakeLength];
	rgbColor snakeColor = { 255, 255, 255 };

	// Level variables
	int currentLevel = 0;
	int pointsToAdvanceLevel = 20;
	int speedIncreasePerLevel = 2;
	int totalPoints = 0;
	int totalPointsThisLevel = 0;
	int screenBoarder = 10;
	float animate_elapsed = 0.0f;
	float animate_rate = 0.03125f;  // 1.0f is 1 second.  0.03125f is  1/32 of a second

	// Fruit variables
	float timeSinceLastFruitSpawn = 0.0f;
	float timeBetweenFruitSpawns = 2.0f; // Spawn 1 item every 2 seconds
	const static int totalFruitAllowed = 5;
	int currentFruitQuantity = 0;
	int fruitPointsValue = 1;
	fruitInstance fruits[totalFruitAllowed];

	void StartLevel()
	{
		currentLevel++;
		totalPointsThisLevel = 0;
		currentFruitQuantity = 0;
	}

	void InitializeSnake()
	{
		snakeBody[0].x = (ScreenWidth() / 2) - 1;
		snakeBody[0].y = ScreenHeight() / 2;
		snakeBody[1].x = ScreenWidth() / 2;
		snakeBody[1].y = ScreenHeight() / 2;
		snakeBody[2].x = (ScreenWidth() / 2) + 1;
		snakeBody[2].y = ScreenHeight() / 2;
	}

	void SpawnFruit()
	{
		// Don't allow us to add more fruit than allowed
		// Also prevents index out of bounds error on fruit object array
		if (currentFruitQuantity >= totalFruitAllowed)
			return;

		// Not enough time has passed, we can't spawn a new one  yet
		if (timeSinceLastFruitSpawn < timeBetweenFruitSpawns) {
			return;
		}

		// Reduce elapsed time by amount of time passed;
		timeSinceLastFruitSpawn = timeSinceLastFruitSpawn - timeBetweenFruitSpawns;

		// Create our new fruit, placing it inside the screen boarders, and forcing it to be bright red.
		fruitInstance fruit;

		fruit.point.x = screenBoarder + (rand() % (ScreenWidth() - (screenBoarder * 2)));
		fruit.point.y = screenBoarder + (rand() % (ScreenHeight() - (screenBoarder * 2)));

		fruit.color.red = 255;
		fruit.color.green = 0;
		fruit.color.blue = 0;

		fruits[currentFruitQuantity] = fruit;

		// Update the number of fruit we have
		currentFruitQuantity++;
	}

	void DrawSnake()
	{
		for (int i = 0; i < snakeLength; i++)
			Draw(snakeBody[i].x, snakeBody[i].y, olc::Pixel(snakeColor.red, snakeColor.green, snakeColor.blue));
	}

	void DrawFruit()
	{
		for (int i = 0; i < currentFruitQuantity; i++)
			Draw(fruits[i].point.x, fruits[i].point.y, olc::Pixel(fruits[i].color.red, fruits[i].color.blue, fruits[i].color.green));
	}

public:
	bool OnUserCreate() override
	{
		InitializeSnake();

		StartLevel();
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		DrawFruit();
		DrawSnake();

		timeSinceLastFruitSpawn += fElapsedTime;
		SpawnFruit();

		animate_elapsed += fElapsedTime;
		if (animate_elapsed > animate_rate) {
			animate_elapsed = animate_elapsed - animate_rate;
			// Handle updates to worm position here
		}

		return true;
	}
};


int main()
{
	SnakeGame demo;
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();

	return 0;
}
