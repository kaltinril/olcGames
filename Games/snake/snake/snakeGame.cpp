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
	struct coords
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

	// Snake variables
	int snakeLength = 3;  // Lets start the snake out with a head, a body, and a tail (length of 3)
	const static int maxSnakeLength = 1000;
	coords snakeBody[maxSnakeLength];
	rgbColor snakeColor = { 255, 255, 255 };

	// Level variables
	int currentLevel = 0;
	int pointsToAdvanceLevel = 20;
	int speedIncreasePerLevel = 2;
	int totalPoints = 0;
	int totalPointsThisLevel = 0;

	// Fruit variables
	float spawnItemsPerSecond = 0.5; // Spawn 1 item every 2 seconds
	int totalFruitAllowed = 5;
	int currentFruitQuantity = 0;
	int fruitPointsValue = 1;


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

	void DrawSnake()
	{
		for (int i = 0; i < snakeLength; i++)
			Draw(snakeBody[i].x, snakeBody[i].y, olc::Pixel(snakeColor.red, snakeColor.green, snakeColor.blue));
	}

	void DrawFruit()
	{

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
