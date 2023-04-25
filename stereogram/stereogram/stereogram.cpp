#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define DISPLAY_DOTS 0
#define DISPLAY_PATTERN 1

// Code written using information explained on this site
// https://www.ime.usp.br/~otuyama/stereogram/basic/index.html
// Re-written after learning more and reading code from this github project:
// https://github.com/synesthesiam/magicpy

class StereoGram : public olc::PixelGameEngine
{
public:

	StereoGram()
	{
		sAppName = "Autostereogram";
	}

	/// <summary>
	///  The dot pattern needs to be created once on the left.  These pixels should not be touched and should not contain mask image data to adjust.
	/// </summary>
	/// <param name="pattern_repetitions">The number of "Sections" of the screen.  Default 10</param>
	void create_dot_pattern(int pattern_repetitions = 10)
	{
		// create the LEFT pattern and draw it also at the RIGHT side
		for (int x = 0; x < ScreenWidth() / pattern_repetitions; x++)
			for (int y = 0; y < ScreenHeight(); y++)
			{
				int color = rand() % 255; // (rand() % 2) * 255;
				int red = color;// (rand() % 2) * 255;
				int green = color;//(rand() % 2) * 255;
				int blue = color;//(rand() % 2) * 255;
				olc::Pixel pix = olc::Pixel(red, green, blue);
				Draw(x, y, pix);
			}
	}

	/// <summary>
	/// Assumption that the threeDImage loaded image is the same size as the screen.
	/// </summary>
	/// <param name="pattern_repetitions"></param>
	void add_stereogram_sprite(int pattern_repetitions = 10, int inverse = 1)
	{
		// Eventually take in a sprite, convert it to greyscale, use the greyscale to change the "offset" or shift of the pixels
		int offset = 0;
		int pattern_width = ScreenWidth() / pattern_repetitions;
		olc::Pixel pix;
		olc::Pixel imagePix;
		// Loop over the remainder of the screen
		// Each "pattern width" wide section is then re-used to sample the data we need for the next "pattern width" section
		// This is a recursive/itterative approach
		for (int y = 0; y < ScreenHeight(); y++)
		{
			for (int x = pattern_width; x < ScreenWidth(); x++)
			{
				imagePix = threeDImage->GetPixel(x, y);
				int greyscale = (imagePix.r + imagePix.g + imagePix.b) / 3; // Just in case someone didn't use greyscale for the image, convert it to greysacale
				offset = greyscale / (255 / pattern_repetitions);

				int samplePixelPosition = (x - pattern_width);
				int shiftedSamplePosition = samplePixelPosition + (offset * inverse);
				pix = PixelGameEngine::GetDrawTarget()->GetPixel(shiftedSamplePosition, y);

				Draw(x, y, pix);
			}
		}
	}

	/// <summary>
	/// This creates a pattern that is repeated instead of random dots that are repeated
	/// </summary>
	/// <param name="pattern_repetitions"></param>
	void stamp_background(int pattern_repetitions = 10)
	{
		int skip_x = backgroundPattern->width;
		int skip_y = backgroundPattern->height;
		for (int y = 0; y < ScreenHeight(); y = y + skip_y)
		{
			for (int x = 0; x < ScreenWidth() / pattern_repetitions; x = x + skip_x)
			{
				DrawSprite(olc::vi2d(x, y), backgroundPattern.get());
			}
		}
	}

private:
	std::unique_ptr<olc::Sprite> threeDImage;
	std::unique_ptr<olc::Sprite> backgroundPattern;
	int state = DISPLAY_DOTS;
	int invert_display = 1;

public:
	bool OnUserCreate() override
	{
		// Load the sprite
		threeDImage = std::make_unique<olc::Sprite>("./800x500.png");
		backgroundPattern = std::make_unique<olc::Sprite>("./background_pattern_tenth.png");

		// Start user off with random background
		create_dot_pattern(10);
		add_stereogram_sprite(10, invert_display);

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		int updateNeeded = 0;

		// Right mouse inverts the image
		if (GetMouse(1).bReleased)
		{
			updateNeeded = 1;
			invert_display *= -1;
		}

		// Left mouse changes between dots and pattern
		if (GetMouse(0).bReleased)
		{
			updateNeeded = 1;
			if (state == DISPLAY_DOTS)
				state = DISPLAY_PATTERN;
			else
				state = DISPLAY_DOTS;
		}

		// If a change happend (a click), then draw the update)
		if (updateNeeded)
		{
			if (state == DISPLAY_DOTS)
			{
				create_dot_pattern(10);
				add_stereogram_sprite(10, invert_display);
			}
			else
			{
				stamp_background(10);
				add_stereogram_sprite(10, invert_display);
			}
		}

		// [Q] Quit
		if (GetKey(olc::Key::Q).bPressed || GetKey(olc::Key::ESCAPE).bPressed)
			return false;

		return true;
	}
};


int main()
{
	StereoGram stereogram;
	bool fullscreen = true;
	bool vsync = false;
	if (stereogram.Construct(800, 500, 2, 2, fullscreen, vsync))
		stereogram.Start();

	return 0;
}