#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

// Override base class with your custom functionality
class Snow : public olc::PixelGameEngine
{
public:
	Snow()
	{
		// Name you application
		sAppName = "Snow";
	}

private:
	int flakes[240][256] = { 0 };
	int frame_skip = 10;
	int frame = 0;

	int snow_fall_rate = 0;
	int max_snow_fall_rate = 50;

	void SpawnSnow(int number_of_flakes) {
		for (int i = 0; i < number_of_flakes; i++)
		{
			int color = 128 + rand() % 128;
			int x = rand() % 256;
			int y = rand() % 2;

			// Only place it if there is room, if the screen fills up we don't wnat to randomly change the pixel color
			if (flakes[y][x] == 0)
				flakes[y][x] = color;
		}
	}

	void MoveSnow()
	{
		int direction = (rand() % 3) - 1;

		for (int y = ScreenHeight() - 2; y >= 0; y--) {
			for (int x = 0; x < ScreenWidth(); x++)
			{

				direction = (rand() % 3) - 1;
				if (flakes[y + 1][x + direction] == 0 && flakes[y][x] != 0) {
					if (y > ScreenHeight() - 6)
						y = y;

					flakes[y + 1][x + direction] = flakes[y][x];
					flakes[y][x] = 0;
				}

			}
		}
	}

	void SpawnSnowAtSpot(int x_in, int y_in) {
		int color = 128 + rand() % 128;
		flakes[y_in][x_in] = color;
	}

public:
	bool OnUserCreate() override
	{
		// Create some random snow flakes
		SpawnSnow(100);

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Spawn new snow
		if (GetMouse(0).bHeld)
			SpawnSnowAtSpot(GetMouseX(), GetMouseY());

		if (GetKey(olc::Key::UP).bPressed) {
			snow_fall_rate++;
			if (snow_fall_rate > max_snow_fall_rate)
				snow_fall_rate = max_snow_fall_rate;
		}

		if (GetKey(olc::Key::DOWN).bPressed) {
			snow_fall_rate--;
			if (snow_fall_rate < 0)
				snow_fall_rate = 0;
		}


		// draw snow
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				Draw(x, y, olc::Pixel(flakes[y][x], flakes[y][x], flakes[y][x]));

		// Move snow down
		frame++;
		if (frame > frame_skip)
		{
			SpawnSnow(snow_fall_rate); // Add some snowfall randomly
			MoveSnow();
			frame = 0;
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