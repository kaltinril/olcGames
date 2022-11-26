#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

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
				int color = (rand() % 2) * 255;
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
		bool debug = false;
		int imgX = 0;
		int imgY = 0;
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

				if (debug == true)
					pix = olc::Pixel(255, 0, 0);

				Draw(x, y, pix);
			}
		}
	}


	void add_stereogram_square()
	{
		int startx = (3 * ScreenWidth() / 4) - ScreenWidth() / 4;
		int endx = (3 * ScreenWidth() / 4) + ScreenWidth() / 4;
		int starty = (ScreenHeight() / 2) - (ScreenHeight() / 5);
		int endy = (ScreenHeight() / 2) + (ScreenHeight() / 5);
		bool debug = false;
		for (int x = startx; x < endx; x++)
			for (int y = starty; y < endy; y++)
			{
				olc::Pixel pix = PixelGameEngine::GetDrawTarget()->GetPixel(x, y);

				if (debug == true)
					if (x == startx || x == endx - 1 || y == starty || y == endy - 1)
						pix = olc::Pixel(255, 0, 0);

				Draw(x - 3, y, pix);
			}
	}


	void stamp_background()
	{
		int skip_x = backgroundPattern->width;
		int skip_y = backgroundPattern->height;
		for (int y = 0; y < ScreenHeight(); y = y + skip_y)
		{
			for (int x = 0; x < ScreenWidth(); x = x + skip_x)
			{
				DrawSprite(olc::vi2d(x, y), backgroundPattern.get());
			}
		}

		/*for (int x = 0; x < ScreenWidth() / 2; x++)
			for (int y = 0; y < ScreenHeight(); y++)
			{
				olc::Pixel pix = PixelGameEngine::GetDrawTarget()->GetPixel(x, y);
				Draw(x + ScreenWidth() / 2, y, pix);
			}
		*/
	}

private:
	std::unique_ptr<olc::Sprite> threeDImage;
	std::unique_ptr<olc::Sprite> backgroundPattern;

public:
	bool OnUserCreate() override
	{
		
		// Load the sprite
		threeDImage = std::make_unique<olc::Sprite>("./800x500.png");
		backgroundPattern = std::make_unique<olc::Sprite>("./background_2.png");

		
		create_dot_pattern(10);
		//stamp_background();
		add_stereogram_sprite(10, 1);
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{

		return true;
	}
};


int main()
{
	StereoGram demo;
	if (demo.Construct(800, 500, 2, 2, false, false))
		demo.Start();

	return 0;
}