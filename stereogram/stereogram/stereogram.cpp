#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class StereoGram : public olc::PixelGameEngine
{
public:
	

	StereoGram()
	{
		sAppName = "Example";
	}

	void create_before_pattern()
	{
		// create the LEFT pattern and draw it also at the RIGHT side
		for (int x = 0; x < ScreenWidth() / 2; x++)
			for (int y = 0; y < ScreenHeight(); y++)
			{
				int color = (rand() % 2) * 255;
				int red = color;// (rand() % 2) * 255;
				int green = color;//(rand() % 2) * 255;
				int blue = color;//(rand() % 2) * 255;
				olc::Pixel pix = olc::Pixel(red, green, blue);
				Draw(x, y, pix);
				Draw(x + ScreenWidth() / 2, y, pix);
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
					if (x == startx || x == endx - 1 || y == starty || y == endy -1)
						pix = olc::Pixel(255, 0, 0);

				Draw(x - 3, y, pix);
			}
	}

	void add_stereogram_sprite()
	{
		// Eventually take in a sprite, convert it to greyscale, use the greyscale to change the "offset" or shift of the pixels
		bool debug = false;
		int imgX = 0;
		int imgY = 0;
		int offset = 0;
		int topLeftDrawX = (3*ScreenWidth() / 4) - (threeDImage->width / 2);
		int topLeftDrawY = (ScreenHeight() / 2) - (threeDImage->height / 2);
		olc::Pixel pix;
		olc::Pixel imagePix;
		for (int imgY = 0; imgY < threeDImage->height; imgY++)
		{
			for (int imgX = 0; imgX < threeDImage->width; imgX++)
			{
				imagePix = threeDImage->GetPixel(imgX, imgY);
				int greyscale = (imagePix.r + imagePix.g + imagePix.b) / 3;

				if (greyscale > 0)
				{
					offset = greyscale / (255 / 5);


					int x = imgX + topLeftDrawX;
					int y = imgY + topLeftDrawY;
					pix = PixelGameEngine::GetDrawTarget()->GetPixel(x, y);

					if (debug == true)
						pix = olc::Pixel(255, 0, 0);
					
					Draw(x - offset, y, pix);
				}
			}
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
		threeDImage = std::make_unique<olc::Sprite>("./star_shaded.png");
		backgroundPattern = std::make_unique<olc::Sprite>("./background_2.png");

		// Called once at the start, so create things here
		create_before_pattern();
		//stamp_background();
		add_stereogram_sprite();
		//add_stereogram_sprite();
		//add_stereogram_sprite2();
		//add_stereogram_sprite3();
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
	if (demo.Construct(300, 200, 2, 2, false, false))
		demo.Start();

	return 0;
}