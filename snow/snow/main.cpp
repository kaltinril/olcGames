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
	int flakes[256][240] = { 0 };

	void SpawnSnow() {
		for (int i = 0; i < 100; i++)
		{
			int color = 128 + rand() % 128;
			int x = rand() % 256;
			int y = rand() % 5;
			flakes[x][y] = color;
		}
	}

public:
	bool OnUserCreate() override
	{
		// Create some random snow flakes
		SpawnSnow();

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// draw snow
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				Draw(x, y, olc::Pixel(flakes[x][y], flakes[x][y], flakes[x][y]));

		// Move snow down

				
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