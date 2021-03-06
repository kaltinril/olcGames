#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define _USE_MATH_DEFINES // So we can get M_PI (pi = 3.1426....)
#include <math.h>

// Override base class with your custom functionality
class Morphv1 : public olc::PixelGameEngine
{
public:
	Morphv1()
	{
		// Name you application
		sAppName = "2D Morph v1";
	}

	// Simple struct to make accessing X/Y easier.
	struct vec2d
	{
		float x, y;
	};

	// Objects
	vec2d* points;
	vec2d* line;
	vec2d* square;
	vec2d* circle;
	vec2d* triangle;
	vec2d* poly;
	int polySides = 5;
	int polyRadius = 60;

	// Object morph order
	vec2d* objects[5];
	int totalObjects = 5;
	int currentObject = 0;
	int nextObject = 1;

	int totalPoints = 1000; // How many points do we want to use?

	// Morphing control
	float transitionTime = 1.0f;
	float currentTransitionTime = 0.0f;
	bool isMorphing = false;
	bool shufflePoints = false;
	

	double twoPi = M_PI * 2; // 360 degrees but in Radians

	void BuildNShapePerimeter(vec2d* points, float centerX, float centerY, int radius, int arrayStart, int numberOfSteps, int numberOfSides)
	{
		int stepsPerSide = numberOfSteps / numberOfSides;
		double angleToNextSide = double(twoPi) / numberOfSides;

		for (int i = 0; i < numberOfSides; i++)
		{
			// Create X and Y around a circle through an incrementing angle
			float x1 = centerX + cos(angleToNextSide * i) * radius;
			float y1 = centerY + sin(angleToNextSide * i) * radius;

			float x2 = centerX + cos(angleToNextSide * (i + 1)) * radius;
			float y2 = centerY + sin(angleToNextSide * (i + 1)) * radius;

			BuildPointsOnLine(points, x1, y1, x2, y2, arrayStart + (i * stepsPerSide), stepsPerSide);
		}
	}

	// Create points along the perimeter of a circle
	void BuildPointsOnCircle(vec2d* points, float centerX, float centerY, int radius, int arrayStart, int numberOfSteps)
	{
		double angleStep = double(twoPi) / numberOfSteps;

		for (int i = 0; i < numberOfSteps; i++)
		{
			// Create X and Y around a circle through an incrementing angle
			float x = cos(angleStep * i);
			float y = sin(angleStep * i);

			points[i].x = centerX + (x * radius);
			points[i].y = centerY + (y * radius);
		}
	}

	// Create points along the slope of a line
	void BuildPointsOnLine(vec2d* points, float x1, float y1, float x2, float y2, int arrayStart, int numberOfSteps)
	{
		float xSlope = (x2 - x1) / float(numberOfSteps - 1);
		float ySlope = (y2 - y1) / float(numberOfSteps - 1);

		for (int i = 0; i < numberOfSteps; i++)
		{
			int arrayPos = arrayStart + i;
			points[arrayPos].x = x1 + (xSlope * i);
			points[arrayPos].y = y1 + (ySlope * i);
		}
	}

	// Build the 4 sides of a rectangle
	void BuildRectangleOutline(vec2d* shapeArray, float x1, float y1, float x2, float y2, int arrayStart, int numberOfSteps)
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

	void BuildShapes()
	{
		BuildPointsOnLine(line, 30, 50, 200, 220, 0, totalPoints);
		BuildRectangleOutline(square, 10, 10, 100, 100, 0, totalPoints);
		BuildPointsOnCircle(circle, 170, 70, 50, 0, totalPoints);
		BuildNShapePerimeter(poly, ScreenWidth() / 2, ScreenHeight() / 2, polyRadius, 0, totalPoints, polySides);
		BuildNShapePerimeter(triangle, ScreenWidth() / 2, ScreenHeight() / 2, 40, 0, totalPoints, 3);
	}

	void ShuffleShapes()
	{
		std::random_shuffle(line, line + totalPoints);
		std::random_shuffle(square, square + totalPoints);
		std::random_shuffle(poly, poly + totalPoints);
		std::random_shuffle(triangle, triangle + totalPoints);
	}

public:
	bool OnUserCreate() override
	{
		// Create storage of the points
		points = new vec2d[totalPoints];
		square = new vec2d[totalPoints];
		circle = new vec2d[totalPoints];
		line = new vec2d[totalPoints];
		poly = new vec2d[totalPoints];
		triangle = new vec2d[totalPoints];

		// Create list of objects to morph between
		objects[0] = line;
		objects[1] = triangle;
		objects[2] = square;
		objects[3] = poly;
		objects[4] = circle;

		// Build all the shapes
		BuildShapes();

		// Randomly shuffle the points so they are not in a straight lne
		if (shufflePoints)
			ShuffleShapes();

		// Start us off in the line position
		points = line;

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLACK);

		// Draw the points
		for (int i = 0; i < totalPoints; i++)
		{
			float colorShade = (250.0f * (float(i) / float(totalPoints))) + 5.0f;
			//Draw(points[i].x, points[i].y, olc::Pixel(colorShade, colorShade, colorShade));
			Draw(points[i].x, points[i].y, olc::WHITE);
		}

		if (GetKey(olc::Key::B).bPressed)
			BuildShapes();

		if (GetKey(olc::Key::S).bPressed)
			ShuffleShapes();

		if (GetMouse(0).bPressed)
			isMorphing = true;


		// Go backwards through shapes
		if (GetMouse(1).bPressed && !isMorphing)
		{
			nextObject = nextObject - 1;

			if (nextObject < 0)
				nextObject = totalObjects - 1;

			if (nextObject == currentObject)
				nextObject = nextObject - 1;

			if (nextObject < 0)
				nextObject = totalObjects - 1;

			isMorphing = true;
		}

		// Perform the per-elasped time morphing
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
				currentObject = nextObject;
				nextObject = (nextObject + 1) % totalObjects;
			}

			currentTransitionTime = currentTransitionTime + fElapsedTime;
		}

		return true;
	}
};

int main()
{
	Morphv1 demo;
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();
	return 0;
}