#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

// Override base class with your custom functionality
class Snow : public olc::PixelGameEngine
{
public:
	Snow()
	{
		sAppName = "Snow";
	}

private:
	int flakes[240][256] = { 0 };
	int snow_fall_rate = 0;
	int max_snow_fall_rate = 50;
	float animate_elapsed = 0.0f;
	float animate_rate = 0.03125f;  // 1.0f is 1 second.  0.03125f is  1/32 of a second


	void SpawnSnow(int number_of_flakes) {
		int color = 0;
		int x = 0;
		int y = 0;
		for (int i = 0; i < number_of_flakes; i++)
		{
			color = 126 + rand() % 128;
			x = rand() % 256;
			y = rand() % 2;
			// Only place it if there is room, if the screen fills up we don't wnat to randomly change the pixel color
			if (flakes[y][x] == 0)
				flakes[y][x] = color;
		}
	}

	void MoveSnow()
	{
		int direction = (rand() % 5) - 2;
		for (int y = ScreenHeight() - 3; y >= 0; y--) {
			for (int x = 0; x < ScreenWidth(); x++)
			{
				direction = (rand() % 3) - 1;
				if (flakes[y + 1][x + direction] == 0 && flakes[y][x] != 0) {
					flakes[y + 1][x + direction] = flakes[y][x];
					flakes[y][x] = 0;
				}
			}
		}
	}

	// Set the snow flakes array position to a random greyscale color from 128-255
	void SpawnSnowAtSpot(int x_in, int y_in) {
		flakes[y_in][x_in] = 126 + rand() % 128;
	}

	// Remove a 5x5 square
	void RemoveSnow(int x_in, int y_in) {
		int ball_size = 3;
		int maxX = std::clamp(x_in + ball_size, 0, ScreenWidth());
		int maxY = std::clamp(y_in + ball_size, 0, ScreenHeight());
		int minX = std::clamp(x_in - ball_size, 0, ScreenWidth());
		int minY = std::clamp(y_in - ball_size, 0, ScreenHeight());

		for (int x = minX; x < maxX; x++)
			for (int y = minY; y < maxY; y++)
				flakes[y][x] = 0;
	}

	// Reset all user changable values and the screen/board
	void ResetBoard() {
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				flakes[y][x] = 0;
		
		snow_fall_rate = 0;
	}

public:
	bool OnUserCreate() override
	{
		// Create some random snow flakes to begin with
		SpawnSnow(25);
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Spawn new snow
		if (GetMouse(0).bHeld)
			SpawnSnowAtSpot(GetMouseX(), GetMouseY());
		
		// Remove snow at spot
		if (GetMouse(1).bHeld)
			RemoveSnow(GetMouseX(), GetMouseY());

		// Increase snow fall rate if UP arrow pressed
		if (GetKey(olc::Key::UP).bPressed) {
			snow_fall_rate++;
			if (snow_fall_rate > max_snow_fall_rate)
				snow_fall_rate = max_snow_fall_rate;
		}

		// Decrease snow fall rate if DOWN arrow pressed
		if (GetKey(olc::Key::DOWN).bPressed) {
			snow_fall_rate--;
			if (snow_fall_rate < 0)
				snow_fall_rate = 0;
		}

		// [R] Reset the board
		if (GetKey(olc::Key::R).bPressed)
			ResetBoard();

		// draw snow
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				Draw(x, y, olc::Pixel(flakes[y][x], flakes[y][x], flakes[y][x]));

		// Move snow down, using elasped time for consitent behavior on faster hardware
		animate_elapsed += fElapsedTime;
		if (animate_elapsed > animate_rate) {
			animate_elapsed = animate_elapsed - animate_rate;
			SpawnSnow(snow_fall_rate); // Add some snowfall randomly
			MoveSnow();
		}

		return true;
	}
};

int main()
{
	Snow snow;
	if (snow.Construct(256, 240, 4, 4))
		snow.Start();
	return 0;
}