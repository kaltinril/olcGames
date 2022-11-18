#include <iostream>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class PixelFire : public olc::PixelGameEngine
{
public:
	PixelFire()
	{
		sAppName = "Fire!";
	}

private:
	olc::Sprite* backBuffer;

	const int WIDTH = 600;
	const int HEIGHT = 400;
	const int SCREEN_SIZE = WIDTH * HEIGHT;
	const int HEIGHT_MINUS_BOTTOM_LINE = 4 * (SCREEN_SIZE - WIDTH);

	// Figure out a better place for this
	struct FirePallet {
		int red;
		int green;
		int blue;
	};
	FirePallet firePallet[256];
	int* fireBuffer = NULL;

	const int BRIGHTNESS_LEVEL = 4; // Range: 1-4.  Setting higher will cause invalid colors.

	int FIREBALLS = 75;
	int fire[75];

	int blurMode = 0;
	int blurType = 0;

	// No reason to optimize this since it will only run once at load time and looping 64 times and doing 64*3 divisions doesn't really matter
	void setFirePal(FirePallet firePallet[])
	{
		for (int a = 0; a < 64; a++)
		{
			firePallet[a].red = a * 4;
			firePallet[a].green = 0;
			firePallet[a].blue = 0;

			firePallet[a + 64].red = 63 * 4;
			firePallet[a + 64].green = a * 4;
			firePallet[a + 64].blue = 0;

			firePallet[a + 128].red = 63 * 4;
			firePallet[a + 128].green = 63 * 4;
			firePallet[a + 128].blue = a * 4;

			firePallet[a + 192].red = 63 * 4;
			firePallet[a + 192].green = 63 * 4;
			firePallet[a + 192].blue = 63 * 4;
		}

	}

	void drawTestLines(FirePallet firePallet[])
	{
		// Draw some points into the buffer
		int x = 0, y = 0;
		for (x = 0; x < WIDTH; x++)
			for (y = 0; y < HEIGHT; y++)
				fireBuffer[(y * WIDTH) + x] = x % 255; // multiplying and adding this many times is not efficent, however this is not drawn except when the mouse is pressed
	}

	void moveFireBalls()
	{
		int n = 0;
		static int yPos = ((ScreenHeight() - 5) * ScreenWidth());

		for (int n2 = 0; n2 < FIREBALLS; ++n2)
		{
			n = rand() % 100;
			if (n > 50)
			{
				fire[n2] = fire[n2] + rand() % 7;
			}
			else
			{
				fire[n2] = fire[n2] - rand() % 7;
			}

			if (fire[n2] > ScreenWidth() - 11) { fire[n2] = (2 * ScreenWidth() / 3) - rand() % (ScreenWidth() / 3); }
			if (fire[n2] < 21) { fire[n2] = (2 * ScreenWidth() / 5) + rand() % (ScreenWidth() / 3); }

			// Left column
			fireBuffer[yPos + fire[n2] - 1 - ScreenWidth()] = 255;
			fireBuffer[yPos + fire[n2] - 1] = 255;
			fireBuffer[yPos + fire[n2] - 1 + ScreenWidth()] = 255;

			// Middle column
			fireBuffer[yPos + fire[n2] - ScreenWidth()] = 255;
			fireBuffer[yPos + fire[n2]] = 255;
			fireBuffer[yPos + fire[n2] + ScreenWidth()] = 255;

			// Right column
			fireBuffer[yPos + fire[n2] + 1 - ScreenWidth()] = 255;
			fireBuffer[yPos + fire[n2] + 1] = 255;
			fireBuffer[yPos + fire[n2] + 1 + ScreenWidth()] = 255;
		}

	}


	void blurScreen()
	{
		int yOffset = 0;
		int arrayOffset = 0;
		int startY = (HEIGHT / 2) * WIDTH;

		// Loop over all pixels on the screen, blurring each
		for (int x = 0; x < WIDTH; x++)
		{
			yOffset = startY;
			for (int y = HEIGHT / 2; y < HEIGHT; y++)
			{
				arrayOffset = yOffset + x;  // First step of optimizing, move it up to the top
				blurPixel(arrayOffset, blurMode);
				yOffset = yOffset + ScreenWidth();
			}
		}
	}

	void blurScreenPockets()
	{
		int yOffset = 0;
		int arrayOffset = 0;

		// Loop over all pixels on the screen, blurring each

		yOffset = 0;
		int startY = rand() % 3;
		int startX = rand() % 3;
		for (int y = startY; y < HEIGHT; y = y + 3)
		{

			for (int x = startX; x < WIDTH; x = x + 3)
			{
				arrayOffset = y * ScreenWidth() + x;  // First step of optimizing, move it up to the top
				blurPixel(arrayOffset, blurMode);
			}
		}
	}

	void blurScreenHalf()
	{
		int yOffset = 0;
		int arrayOffset = 0;

		// Loop over all pixels on the screen, blurring each
		for (int x = 0; x < WIDTH; x = x + 1)
		{
			yOffset = 0;
			for (int y = 0; y < HEIGHT; y = y + 2)
			{
				arrayOffset = yOffset + x;  // First step of optimizing, move it up to the top
				blurPixel(arrayOffset, blurMode);
				yOffset = yOffset + ScreenWidth();
			}
		}

	}

	void blurPixel(int arrayOffset, int mode)
	{
		int rgbCombined = 0;

		int offsetTop = arrayOffset - ScreenWidth();
		int offsetBottom = arrayOffset + ScreenWidth();

		// [L] [M] [R]
		// [L] [M] [R]
		// [L] [M] [R]

		// Middle column
		rgbCombined += getColorPalletValue(offsetTop); //  x, y - 1);
		//rgbCombined += getColorPalletValue(arrayOffset); //x, y); // ignore my color
		rgbCombined += getColorPalletValue(offsetBottom);//x, y + 1);

														 // Left column
		rgbCombined += getColorPalletValue(offsetTop - 1); // x - 1, y - 1);
		rgbCombined += getColorPalletValue(arrayOffset - 1); // x - 1, y);
		rgbCombined += getColorPalletValue(offsetBottom - 1); // x - 1, y + 1);

															  // Right column
		rgbCombined += getColorPalletValue(offsetTop + 1); // x + 1, y - 1);
		rgbCombined += getColorPalletValue(arrayOffset + 1); // x + 1, y);
		rgbCombined += getColorPalletValue(offsetBottom + 1); // x + 1, y + 1);


		// Average the colors together
		if (mode == 0 || mode == 1)
			rgbCombined = rgbCombined >> 3; // bitshift by 3 which is divide by 8, not exact but close
		else if (mode == 2)
		{
			rgbCombined += getColorPalletValue(arrayOffset); //include my color (this point/pixel)
			rgbCombined = rgbCombined >> 3;
			rgbCombined = rgbCombined >> 2;
		}

		// // bitshift by 3 which is divide by 8, not exact but close

		if (mode == 1) {
			rgbCombined = rgbCombined - 4;
			if (rgbCombined < 0) rgbCombined = 0;
		}

		// Set the color of the pixel
		fireBuffer[arrayOffset] = rgbCombined;
	}

	int getColorPalletValue(int arrayOffset)
	{
		// Don't get the value if it's outside of the screen
		// Return false for other functions to know we didn't get a value
		if ((arrayOffset < 0) || (arrayOffset >= SCREEN_SIZE))
			return 0;

		return fireBuffer[arrayOffset];
	}

	void transferToScreen()
	{
		// Draw from the image to the screen
		int place = 0;
		int yOffset = 0;
		for (int x = 0; x < ScreenWidth(); x++)
		{
			yOffset = 0;
			for (int y = 0; y < ScreenHeight(); y++)
			{
				place = yOffset + x;

				//backBuffer->SetPixel(x, y, olc::Pixel(firePallet[fireBuffer[place]].red, firePallet[fireBuffer[place]].green, firePallet[fireBuffer[place]].blue));
				Draw(x, y, olc::Pixel(firePallet[fireBuffer[place]].red, firePallet[fireBuffer[place]].green, firePallet[fireBuffer[place]].blue));
				yOffset = yOffset + ScreenWidth(); //(Doing this instead of Y * WIDTH)
			}
		}
	}

	void moveThemUp()
	{
		memcpy(fireBuffer, fireBuffer + WIDTH, HEIGHT_MINUS_BOTTOM_LINE);
	}

public:
	bool OnUserCreate() override
	{
		backBuffer = new olc::Sprite(ScreenWidth(), ScreenHeight());
		fireBuffer = new int[SCREEN_SIZE];
		memset(fireBuffer, 0, 4 * SCREEN_SIZE);	// Clear the array

		for (int i = 0; i < FIREBALLS; ++i)
		{
			fire[i] = (ScreenWidth() / 2) - FIREBALLS + i;
		}

		// Fire specific
		setFirePal(firePallet);

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{

		if (GetMouse(0).bReleased)
		{
			blurMode++;
			blurMode = blurMode % 3;
		}

		if (GetMouse(1).bReleased)
		{
			blurType++;
			blurType = blurType % 3;
		}

		//drawTestLines(firePallet);

		moveFireBalls();			// Make the fire seem more real than just random dots and noise.

		if (blurType == 0)
			blurScreen();			// Best, but slowest
		else if (blurType == 1)
			blurScreenPockets();	// Interesting to play with
		else
			blurScreenHalf();		// Odd, like a fuse being lit

		moveThemUp();
		transferToScreen(); // Transfer to the screen using Draw
		return true;
	}
};


int main()
{
	PixelFire demo;
	if (demo.Construct(600, 400, 2, 2, true))
		demo.Start();

	return 0;
}
