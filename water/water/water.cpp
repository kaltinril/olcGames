#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

// Override base class with your custom functionality
class Water : public olc::PixelGameEngine
{
public:
	Water()
	{
		sAppName = "Water";
	}

private:
	int drops[240][256] = { 0 };
	int water_fall_rate = 0;
	int max_water_fall_rate = 50;
	float animate_elapsed = 0.0f;
	float animate_rate = 0.0078125f;  // 1.0f is 1 second.  0.03125f is  1/32 of a second
	bool left = true;


	void SpawnWater(int number_of_drops) {
		int color = 0;
		int x = 0;
		int y = 0;
		for (int i = 0; i < number_of_drops; i++)
		{
			color = 126 + rand() % 128;
			x = rand() % 256;
			y = rand() % 2;
			// Only place it if there is room, if the screen fills up we don't wnat to randomly change the pixel color
			if (drops[y][x] == 0)
				drops[y][x] = color;
		}
	}

	void MoveWater(bool left)
	{
		int direction = rand() % 2;
		if (direction == 1)
			rightToLeft();
		else
			leftToRight();
	}

	void leftToRight()
	{
		for (int y = ScreenHeight() - 3; y >= 0; y--)
			for (int x = 1; x < ScreenWidth() - 2; x++)
				MoveWaterDrop(y, x);
	}

	void rightToLeft()
	{
		for (int y = ScreenHeight() - 3; y >= 0; y--)
			for (int x = ScreenWidth() - 2; x > 1; x--)
				MoveWaterDrop(y, x);
	}

	void MoveWaterDrop(int y, int x)
	{
		if (drops[y][x] == 0 || drops[y][x] == 100)
			return;

		// If the spot directly below is empty, move there first.
		if (drops[y + 1][x] == 0)
		{
			drops[y + 1][x] = drops[y][x];
			drops[y][x] = 0;
		}
		else if (drops[y + 1][x + 1] == 0)
		{
			drops[y + 1][x + 1] = drops[y][x];
			drops[y][x] = 0;
		}
		else if (drops[y + 1][x - 1] == 0)
		{
			drops[y + 1][x - 1] = drops[y][x];
			drops[y][x] = 0;
		}
		else
		{
			// Lets see if we can move randomly out left/right from the spot up to 5 max places if there is nothing in the way.
			int spread = rand() % 5;
			for (int i = spread; i > -spread; i--)
			{
				if (x + i < 1 || x + i > ScreenWidth() - 1)
					break;

				if (drops[y][x + i] == 1) // "Wall" can't move
					break;

				if (drops[y][x + i] == 0)
				{
					// found a spot, move me
					drops[y][x + i] = drops[y][x];
					drops[y][x] = 0;
					break;
				}
			}
		}
	}

	// Set the water drops array position to a random greyscale color from 128-255
	void SpawnWaterAtSpot(int x_in, int y_in) {
		int ball_size = 3;
		int maxX = std::clamp(x_in + ball_size, 0, ScreenWidth());
		int maxY = std::clamp(y_in + ball_size, 0, ScreenHeight());
		int minX = std::clamp(x_in - ball_size, 0, ScreenWidth());
		int minY = std::clamp(y_in - ball_size, 0, ScreenHeight());

		
		for (int y = minY; y < maxY; y++)
		{
			for (int x = minX; x < maxX; x++)
			{
				drops[y][x] = 126 + rand() % 128;
			}
		}
		
	}

	// Remove a square
	void RemoveWater(int x_in, int y_in) {
		int ball_size = 15;
		int maxX = std::clamp(x_in + ball_size, 0, ScreenWidth());
		int maxY = std::clamp(y_in + ball_size, 0, ScreenHeight());
		int minX = std::clamp(x_in - ball_size, 0, ScreenWidth());
		int minY = std::clamp(y_in - ball_size, 0, ScreenHeight());

		for (int x = minX; x < maxX; x++)
			for (int y = minY; y < maxY; y++)
				drops[y][x] = 0;
	}

	// Reset all user changable values and the screen/board
	void ResetBoard() {
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				drops[y][x] = 0;

		water_fall_rate = 0;
	}

public:
	bool OnUserCreate() override
	{
		// Create some random water drops to begin with
		SpawnWater(25);
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLACK);

		// Spawn new water
		if (GetMouse(0).bHeld)
			SpawnWaterAtSpot(GetMouseX(), GetMouseY());

		// Remove water at spot
		if (GetMouse(1).bHeld)
			RemoveWater(GetMouseX(), GetMouseY());

		// Increase water fall rate if UP arrow pressed
		if (GetKey(olc::Key::UP).bPressed) {
			water_fall_rate++;
			if (water_fall_rate > max_water_fall_rate)
				water_fall_rate = max_water_fall_rate;
		}

		// Decrease water fall rate if DOWN arrow pressed
		if (GetKey(olc::Key::DOWN).bPressed) {
			water_fall_rate--;
			if (water_fall_rate < 0)
				water_fall_rate = 0;
		}

		// Increase frame rate if RIGHT arrow pressed
		if (GetKey(olc::Key::RIGHT).bHeld || GetKey(olc::Key::RIGHT).bPressed) {
			animate_rate = animate_rate - 0.0001;
			if (animate_rate < 0)
				animate_rate = 0;
		}

		// Increase frame rate if LEFT arrow pressed
		if (GetKey(olc::Key::LEFT).bHeld || GetKey(olc::Key::LEFT).bPressed) {
			animate_rate = animate_rate + 0.0001;
			if (animate_rate > 0.5f)
				animate_rate = 0.5f;
		}

		// [R] Reset the board
		if (GetKey(olc::Key::R).bPressed)
			ResetBoard();

		// draw water
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				if (drops[y][x] != 0) // Only draw the pixel if it's not black
					Draw(x, y, olc::Pixel(0, 0, drops[y][x]));

		// Move water down, using elasped time for consitent behavior on faster hardware
		animate_elapsed += fElapsedTime;
		if (animate_elapsed > animate_rate) {
			animate_elapsed = animate_elapsed - animate_rate;
			SpawnWater(water_fall_rate); // Add some waterfall randomly
			
			
			MoveWater(left);
		}

		return true;
	}
};

int main()
{
	Water water;
	if (water.Construct(256, 240, 4, 4))
		water.Start();
	return 0;
}