#include <iostream>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class Snake : public olc::PixelGameEngine
{
public:
	Snake()
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
	Snake demo;
	if (demo.Construct(600, 400, 2, 2))
		demo.Start();

	return 0;
}
