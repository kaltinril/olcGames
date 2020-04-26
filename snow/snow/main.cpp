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

	void SpawnSnow(int number_of_flakes) {
		for (int i = 0; i < number_of_flakes; i++)
		{
			int color = 128 + rand() % 128;
			int x = rand() % 256;
			int y = rand() % 2;
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
		SpawnSnow(5);

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Spawn new snow
		if (GetMouse(0).bHeld)
		{
			SpawnSnowAtSpot(GetMouseX(), GetMouseY());
		}


		// draw snow
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				Draw(x, y, olc::Pixel(flakes[y][x], flakes[y][x], flakes[y][x]));

		// Move snow down
		frame++;
		if (frame > frame_skip)
		{
			SpawnSnow(1);
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