#include <iostream>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
// A simple "snake" clone.  Or "worms.io" for the new kids


class SnakeGame : public olc::PixelGameEngine
{
public:
	SnakeGame()
	{
		sAppName = "Yummy!";
	}

public:
	bool OnUserCreate() override
	{
		
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{

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
