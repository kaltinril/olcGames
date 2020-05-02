#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

// Override base class with your custom functionality
class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		// Name you application
		sAppName = "Example";
	}

	struct vec2d
	{
		float x, y;
	};

	vec2d* points;
	vec2d* line;
	vec2d* square;
	vec2d* circle;

	vec2d* objects[3];
	int currentObject = 0;
	int nextObject = 1;

	float transitionTime = 1.0f;
	float currentTransitionTime = 0.0f;
	bool isMorphing = false;
	int totalPoints = 1000;

	void BuildPointsOnCircle(vec2d* points, float centerX, float centerY, int radius, int arrayStart, int numberOfSteps)
	{

	}

	void BuildPointsOnLine(vec2d* points, float x1, float y1, float x2, float y2, int arrayStart, int numberOfSteps)
	{
		//vec2d* points = new vec2d[numberOfSteps];

		float xSlope = (x2 - x1) / float(numberOfSteps - 1);
		float ySlope = (y2 - y1) / float(numberOfSteps - 1);

		//int endLocation = arrayStart + numberOfSteps;
		for (int i = 0; i < numberOfSteps; i++)
		{
			int arrayPos = arrayStart + i;
			points[arrayPos].x = x1 + (xSlope * i);
			points[arrayPos].y = y1 + (ySlope * i);
		}
	}

	void BuildSquareOutline(vec2d* shapeArray, float x1, float y1, float x2, float y2, int arrayStart, int numberOfSteps)
	{
		int stepsPerSide = numberOfSteps / 4;

		BuildPointsOnLine(shapeArray, x1, y1, x1, y2, arrayStart + (0 * stepsPerSide), stepsPerSide);
		BuildPointsOnLine(shapeArray, x1, y1, x2, y1, arrayStart + (1 * stepsPerSide), stepsPerSide);
		BuildPointsOnLine(shapeArray, x2, y1, x2, y2, arrayStart + (2 * stepsPerSide), stepsPerSide);
		BuildPointsOnLine(shapeArray, x1, y2, x2, y2, arrayStart + (3 * stepsPerSide), stepsPerSide);
	}

	// Slow transition the particles from drawnShape into targetShape
	vec2d* MorphPartial(vec2d* currentShape, vec2d* targetShape, float percentComplete, int numberOfPoints)
	{
		vec2d* points = new vec2d[numberOfPoints];

		for (int i = 0; i < numberOfPoints; i++)
		{
			float xSlope = (targetShape[i].x - currentShape[i].x) * percentComplete;
			float ySlope = (targetShape[i].y - currentShape[i].y) * percentComplete;

			points[i].x = currentShape[i].x + xSlope;
			points[i].y = currentShape[i].y + ySlope;
		}

		return points;
	}

public:
	bool OnUserCreate() override
	{
		
		points = new vec2d[totalPoints];
		square = new vec2d[totalPoints];
		circle = new vec2d[totalPoints];
		line = new vec2d[totalPoints];

		objects[0] = line;
		objects[1] = square;
		objects[2] = circle;

		BuildPointsOnLine(line, 30, 50, 200, 220, 0, totalPoints);
		BuildPointsOnLine(points, 30, 50, 200, 220, 0, totalPoints);
		BuildSquareOutline(square, 10, 10, 100, 100, 0, totalPoints);
		BuildSquareOutline(circle, 50, 50, 175, 125, 0, totalPoints);

		// Randomly shuffle the points so they are not in a straight lne
		std::random_shuffle(points, points + totalPoints);
		std::random_shuffle(line, line + totalPoints);
		std::random_shuffle(square, square + totalPoints);
		std::random_shuffle(circle, circle + totalPoints);
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLACK);

		for (int i = 0; i < totalPoints; i++)
			Draw(points[i].x, points[i].y, olc::WHITE);

		if (GetMouse(0).bPressed)
			isMorphing = true;

		if (isMorphing)
		{
			float percentComplete = currentTransitionTime / transitionTime;
			if (percentComplete >= 1.0f)
			{
				percentComplete = 1.0f;
				isMorphing = false;
			}

			points = MorphPartial(objects[currentObject], objects[nextObject], percentComplete, totalPoints);

			if (!isMorphing) {
				currentTransitionTime = 0.0f;
				currentObject = (currentObject + 1) % 3;
				nextObject = (nextObject + 1) % 3;
			}

			currentTransitionTime = currentTransitionTime + fElapsedTime;
		}

		//Draw(11, 11, olc::RED);
		//Draw(51, 51, olc::RED);
		//Draw(101, 101, olc::RED);
		//Draw(141, 141, olc::RED);

		return true;
	}
};

int main()
{
	Example demo;
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();
	return 0;
}