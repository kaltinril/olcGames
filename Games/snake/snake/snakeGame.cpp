#include <iostream>
#include <string>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
// A simple "snake" clone.  Or "worms.io" for the new kids
// Library from One Lone Coder
// Snake Game code by Kaltinril
// This is the non-OO version
// We use a struct for the X/Y position of each snake body part
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

	// fuitInstance can be expanded to include more data like points or value or lifetime on how long it lives
	struct fruitInstance
	{
		point2D point;
		rgbColor color;
	};

	// Snake variables
	int snakeLength = 10;  // Lets start the snake out with a head, a body, and a tail (length of 3)
	const static int maxSnakeLength = 500;
	point2D snakeBody[maxSnakeLength];
	rgbColor snakeColor = { 0, 255, 255 };
	int xDir = -1;
	int yDir = 0;

	// Level variables
	bool gameIsOver = false;
	bool levelIsPaused = true;
	int currentLevel = 0;
	int pointsToAdvanceLevel = 10;
	int totalPoints = 0;
	int totalPointsThisLevel = 0;
	int screenBoarder = 0;
	float animate_elapsed = 0.0f;
	float animate_rate = 0.04f;  // 1.0f is 1 second.  0.03125f is  1/32 of a second  1/30 = 0.01666f  1/16 = 0.0625

	// Fruit variables
	float timeSinceLastFruitSpawn = 0.0f;
	float timeBetweenFruitSpawns = 2.0f; // Spawn 1 item every 2 seconds
	int totalFruitAllowed = 5;
	int currentFruitQuantity = 0;
	int fruitPointsValue = 1;
	fruitInstance* fruits;

	void StartLevel()
	{
		currentLevel++;
		ResetLevel();
	}

	void ResetLevel()
	{
		totalPointsThisLevel = 0;
		currentFruitQuantity = 0;
	}

	void ResetGame()
	{
		animate_rate = 0.04f;
		currentLevel = 1;
		totalPoints = 0;

		timeBetweenFruitSpawns = 2.0f;
		timeSinceLastFruitSpawn = 0.0f;
		totalFruitAllowed = 5;
		currentFruitQuantity = 0;
		ResetLevel();

		snakeLength = 10;
		InitializeSnake();
	}

	void InitializeSnake()
	{
		snakeBody[0].x = (ScreenWidth() / 2) - 1;
		snakeBody[0].y = ScreenHeight() / 2;

		for (int i = 1; i < snakeLength; i++)
		{
			snakeBody[i].x = snakeBody[i - 1].x + 1;
			snakeBody[i].y = snakeBody[i - 1].y;
		}
	}

	// We'll only check the head
	void CheckForFruitCollision()
	{
		int fruitCollision = -1;

		// Check for a collision with the head against all current fruit
		for (int f = 0; f < currentFruitQuantity; f++)
		{
			if (snakeBody[0].x == fruits[f].point.x && snakeBody[0].y == fruits[f].point.y)
			{
				fruitCollision = f;
				break;
			}
		}

		// if we had a collision, eat the fruit, and then remove the fruit
		if (fruitCollision != -1)
		{
			EatFruitAtIndex(fruitCollision);
			RemoveFruitAtIndex(fruitCollision);
		}
	}

	void EatFruitAtIndex(int fruitIndex)
	{
		// Currently no reason to have this method, however, if you add different types of fruit
		// here is where you would put the logic for what each can do
		// This might make it easier to track the logic, and eventually, move it into classes
		totalPointsThisLevel += fruitPointsValue;
		totalPoints += fruitPointsValue;
		snakeLength++;
	}

	void RemoveFruitAtIndex(int fruitIndex)
	{
		// loop over all fruit in the array and move them down one
		for (int f = fruitIndex; f < currentFruitQuantity - 1; f++)
			fruits[f] = fruits[f + 1];

		currentFruitQuantity--;
	}

	void UpdateFruit()
	{
		// Add logic here for fruit disappearing after N seconds
		// or maybe the fruit stays but slowly rots and turns black
		// or maybe you just want to animate the fruit
		
		//for (int f = 0; f < currentFruitQuantity; f++)
		//{
		//	fruits[f]
		//}
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

	void CheckUserInput()
	{
		// Allow snake to move, but we can't turn back ontop of ourself
		if (GetKey(olc::Key::LEFT).bPressed && xDir != 1)
		{
			xDir = -1;
			yDir = 0;
		}
		if (GetKey(olc::Key::RIGHT).bPressed && xDir != -1)
		{
			xDir = 1;
			yDir = 0;
		}
		if (GetKey(olc::Key::UP).bPressed && yDir != 1)
		{
			xDir = 0;
			yDir = -1;
		}
		if (GetKey(olc::Key::DOWN).bPressed && yDir != -1)
		{
			xDir = 0;
			yDir = 1;
		}
		if (GetKey(olc::Key::SPACE).bPressed)
		{
			levelIsPaused = !levelIsPaused; // if true, becomes false, if false, becomes true (Invert)

			// Special case, if the game was over, clear pause also when they press it
			if (gameIsOver)
			{
				gameIsOver = false;
				levelIsPaused = false;
				ResetGame();
			}
		}
	}

	void ChangeLevelsIfComplete()
	{
		if (totalPointsThisLevel > pointsToAdvanceLevel)
		{
			timeBetweenFruitSpawns = timeBetweenFruitSpawns - 0.05;
			totalFruitAllowed++;
			animate_rate = animate_rate - 0.001f;
			StartLevel();
		}
	}

	// Expand this to include different types of fruits perhaps.
	void DrawFruit()
	{
		for (int i = 0; i < currentFruitQuantity; i++)
			Draw(fruits[i].point.x, fruits[i].point.y, olc::Pixel(fruits[i].color.red, fruits[i].color.blue, fruits[i].color.green));
	}

	void DrawSnake()
	{
		// Draw tail first
		for (int i = snakeLength - 1; i >= 0; i--)
		{
			// Create a "greyscale" affect on the worm
			int red = snakeColor.red * (1.0f - (i / (snakeLength - 1.0f)));
			int green = snakeColor.green * (1.0f - (i / (snakeLength - 1.0f)));
			int blue = snakeColor.blue * (1.0f - (i / (snakeLength - 1.0f)));
			Draw(snakeBody[i].x, snakeBody[i].y, olc::Pixel(red , green , blue ));
		}
	}

	void DrawBorder()
	{
		int width = ScreenWidth() - ((screenBoarder * 2) + 1);		// * 2 because left and right border
		int height = ScreenHeight() - ((screenBoarder * 2) + 1);	// sutract another 1 because the border itself is 1 pixel
		DrawRect(screenBoarder, screenBoarder, width, height, olc::WHITE);
	}

	void DrawLevelStats()
	{
		// Make the text "transparent-ish"
		SetPixelMode(olc::Pixel::ALPHA);

		// Draw Points
		std::string stringToDraw = std::to_string(totalPoints);
		olc::Pixel transparentYellow = olc::Pixel(255, 255, 0, 196);
		DrawStringPropDecal({ 2, 2 }, stringToDraw, transparentYellow, { 1, 1 });

		// Draw Level
		stringToDraw = std::to_string(currentLevel);
		DrawStringPropDecal({ (float)ScreenWidth() - 10, 2 }, stringToDraw, transparentYellow, { 1, 1 });

		// Reset pixel mode to normal
		SetPixelMode(olc::Pixel::NORMAL);
	}

	void DrawGameOverScreen()
	{
		DrawStringPropDecal({ 14, (float)(ScreenHeight() / 2) - 10 }, "GAME",  olc::RED, { 1, 1 });
		DrawStringPropDecal({ 14, (float)(ScreenHeight() / 2) }		, "OVER",  olc::RED, { 1, 1 });
		DrawStringPropDecal({ 12, (float)(ScreenHeight() / 2) + 10 }, "PRESS", olc::YELLOW, { 1, 1 });
		DrawStringPropDecal({ 12, (float)(ScreenHeight() / 2) + 20 }, "SPACE", olc::YELLOW, { 1, 1 });
	}

	void UpdateWorm()
	{
		// Starting with the tail
		// move each point of the snake 
		for (int i = snakeLength - 1; i > 0; i--)
			snakeBody[i] = snakeBody[i - 1];

		// Set the new head position "move the head"
		snakeBody[0].x = snakeBody[0].x + xDir;
		snakeBody[0].y = snakeBody[0].y + yDir;
	}

	void CheckIfOutOfBounds()
	{
		if (   snakeBody[0].x <= screenBoarder || snakeBody[0].x >= ScreenWidth() - screenBoarder
			|| snakeBody[0].y <= screenBoarder || snakeBody[0].y >= ScreenHeight() - screenBoarder)
			gameIsOver = true;
	}

public:
	bool OnUserCreate() override
	{
		InitializeSnake();

		fruits = new fruitInstance[totalFruitAllowed];

		StartLevel();
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLACK);

		CheckUserInput();

		// If esc is pressed, quit
		if (GetKey(olc::Key::ESCAPE).bPressed)
			return false;

		DrawBorder();
		DrawFruit();
		DrawSnake();
		DrawLevelStats();

		CheckForFruitCollision();
		UpdateFruit();
		timeSinceLastFruitSpawn += fElapsedTime;

		CheckIfOutOfBounds();

		// no reason to update the worm position, or spawn new fruit if the level is paused
		// All logic below this IF statement should only be logic that will run
		// when the level is UN-PAUSED
		if (gameIsOver)
		{
			DrawGameOverScreen();
			return true;
		}
		else if (levelIsPaused)
		{
			DrawStringPropDecal({ 12, (float)(ScreenHeight() / 2) + 10 }, "PRESS", olc::YELLOW, {1, 1});
			DrawStringPropDecal({ 12, (float)(ScreenHeight() / 2) + 20 }, "SPACE", olc::YELLOW, {1, 1});
			return true;
		}

		SpawnFruit();

		animate_elapsed += fElapsedTime;
		if (animate_elapsed > animate_rate) {
			animate_elapsed = animate_elapsed - animate_rate;
			// Handle updates to worm position here
			UpdateWorm();
		}

		ChangeLevelsIfComplete();

		return true;
	}
};


int main()
{
	SnakeGame demo;
	// Lets make large pixels for fun
	bool fullScreen = true;
	if (demo.Construct(64, 64, 16, 16, fullScreen))
		demo.Start();

	return 0;
}
