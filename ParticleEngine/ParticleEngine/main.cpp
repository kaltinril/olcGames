#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <list>
#include <time.h>
#define _USE_MATH_DEFINES // So we can get M_PI (pi = 3.1426....)
#include <math.h>

// Override base class with your custom functionality
class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		// Name you application

		sAppName = "Example";
	}

	struct Renderable
	{
		Renderable() {}

		void Load(const std::string& sFile)
		{
			sprite = new olc::Sprite(sFile);
			decal = new olc::Decal(sprite);
		}

		~Renderable()
		{
			delete decal;
			delete sprite;
		}

		olc::Sprite* sprite = nullptr;
		olc::Decal* decal = nullptr;
	};


	struct vec2d
	{
		float x, y;

		// Not optimized, if performance suffers, will need to optimize
		void normalize()
		{
			float mag = sqrt((x * x) + (y * y));
			x = x / mag;
			y = y / mag;
		}
	};

	static int randBetween(int min, int max)
	{
		int range = max - min;
		return (rand() % range) + min;
	}

	static float randBetween(float min, float max)
	{
		float range = max - min;
		float value = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		return (value * range) + min;
	}


	class Particle
	{
	public:
		Particle()
		{
		
		}

		void Update(olc::PixelGameEngine* pge, float dElapsedTime)
		{
			// Check if particle should be killed
			if (position.x > pge->ScreenWidth()
				|| position.x < 0
				|| position.y > pge->ScreenHeight()
				|| position.y < 0
				|| current_lifetime > max_lifetime)
			{
				alive = false;
			}


			if (alive)
			{
				// Increase how long this particle has been alive
				current_lifetime += dElapsedTime;

				// Update Velocity based on acceleration for this time slice (dElapsedTime)
				velocity.x = velocity.x + (acceleration.x * dElapsedTime);
				velocity.y = velocity.y + (acceleration.y * dElapsedTime);

				// Update position based on the current velocity in this given time slice (dElapsedTime)
				position.x = position.x + (velocity.x * dElapsedTime);
				position.y = position.y + (velocity.y * dElapsedTime);

				// Adjust size and alpha based on time alive
				float normalizedLifetime = current_lifetime / max_lifetime;

				// we want particles to fade in and fade out, so we'll calculate alpha
				// to be (normalizedLifetime) * (1-normalizedLifetime). this way, when
				// normalizedLifetime is 0 or 1, alpha is 0. the maximum value is at
				// normalizedLifetime = .5, and is
				// (normalizedLifetime) * (1-normalizedLifetime)
				// (.5)                 * (1-.5)
				// .25
				// since we want the maximum alpha to be 1, not .25, we'll scale the 
				// entire equation by 4.
				int alpha = int((4 * normalizedLifetime * (1 - normalizedLifetime)) * 256);
				//int alpha = int((1 - (4 * normalizedLifetime * (1 - normalizedLifetime))) * 256);

				// This looks better
				alpha = (1-normalizedLifetime) * 256;
				
				color.a = alpha;
				color.r = alpha * ( 0.80); // Making it slightly purple
				color.g = alpha * (0.25); // Making it slightly purple
				color.b = alpha;

				scale = (0.75f + (.25f * normalizedLifetime));
				//pge->DrawDecal({ position.x, position.y }, sprite->decal, { scale, scale }, color);
			}
		}

		void Draw(olc::PixelGameEngine* pge)
		{
			if (alive)
				pge->DrawDecal({ position.x, position.y }, sprite->decal, { scale, scale }, color);
		}

	public:
		// The image of the particle
		Renderable* sprite;
		olc::Decal* decal = nullptr;
		olc::Pixel color; // Control tint and alpha of sprite with this
		float scale = 1.0f; // 1.0 = 100% // 2.0 = 200%

		// Control the movement of the particle
		vec2d velocity;
		vec2d acceleration;
		vec2d position;

		// Control how long the particle lives
		float max_lifetime = 1.0f;
		float current_lifetime = 0.0f;
		bool alive = false;
		/*
			<BlendState>Additive</BlendState>
		<TextureName>ParticleImages/explosion</TextureName>
		<MinNumParticles>20</MinNumParticles>
		<MaxNumParticles>25</MaxNumParticles>
		<MinLifetime>0.5</MinLifetime>
		<MaxLifetime>1.0</MaxLifetime>
		<MinInitialSpeed>40</MinInitialSpeed>
		<MaxInitialSpeed>500</MaxInitialSpeed>
		<MinAcceleration>0</MinAcceleration>
		<MaxAcceleration>0</MaxAcceleration>
		<MinRotationSpeed>-0.78539816339744830961566084581988</MinRotationSpeed>
		<MaxRotationSpeed>0.78539816339744830961566084581988</MaxRotationSpeed>
		<MinScale>0.3</MinScale>
		<MaxScale>1.0</MaxScale>
		<Direction>0 0</Direction>
		<ParticlesPerFrame>2</ParticlesPerFrame>
		<RandomColor>0</RandomColor>

		*/
	};


	// O------------------------------------------------------------------------------O
	// | olc::ParticleEngine - The main class that keeps track of all the particles   |
	// O------------------------------------------------------------------------------O
	class ParticleEngine
	{
	public:
		ParticleEngine()
		{

		}

		void Load()
		{
			darkCircle.Load("./gfx/particles/bluedark.png");
		}

		void Update(olc::PixelGameEngine* pge, float dElapsedTime)
		{
			GenerateParticles(pge);

			for (int i = particles.size() - 1; i >= 0; i--)
			{
				if (particles[i].alive)
				{
					particles[i].Update(pge, dElapsedTime);
				}
				else
				{
					// NOTE: This is not efficent.
					// We need to remove dead particles
					// Perhaps we need to implement an array instead.
					particles.erase(particles.begin() + i);
					
				}
			}

			for (Particle& particle : particles)
				particle.Update(pge, dElapsedTime);
		}

		void Draw(olc::PixelGameEngine* pge)
		{
			for (Particle& particle : particles)
				particle.Draw(pge);
				
		}

		// Abstract this to shapes or patterns
		// for now going to try to re-create the shield effect from topfalling
		void GenerateParticle(olc::PixelGameEngine* pge)
		{
			
			// Pick position of particle on circle
			double nextFloat = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			double angle = nextFloat * double(twoPi);

			float x = cos(angle);
            float y = sin(angle);

			Particle p;
			int radius = 40;
			p.position.x = (x * radius) + 100;
			p.position.y = (y * radius) + 100;

			int dir = rand() % 2;
			if (dir == 0)
				dir = -1;
			p.velocity = { randBetween(10.0f, 30.0f) * x * dir, randBetween(10.0f, 30.0f) * y * dir };
			p.acceleration = { randBetween(30.0f, 50.0f) * x * dir, randBetween(30.0f, 50.0f) * y * dir };
			

			p.color = olc::Pixel(255, 255, 255, 255);
			p.alive = true;
			p.scale = randBetween(0.5f, 0.7f);
			

			p.sprite = &darkCircle;
			p.max_lifetime = randBetween(0.2f, 0.7f);


			particles.push_back(p);
		}

		void GenerateParticles(olc::PixelGameEngine* pge) {
			if (particles.size() < maxParticles)
			{
				int missing = maxParticles - particles.size();
				for (int i = 0; i < missing; i++)
					GenerateParticle(pge);
			}

		}

	public:
		double twoPi = M_PI * 2;
		std::vector<Particle> particles;
		Renderable darkCircle;
		int maxParticles = 700;

	};

	ParticleEngine particleEngine;
	Renderable diamond;
	Renderable star;
	Renderable fire;
	Renderable smoke;
	Renderable circle;
	Renderable* img[5];

public:
	bool OnUserCreate() override
	{
		// Load all particle images
		diamond.Load("./gfx/particles/diamond.png");
		star.Load("./gfx/particles/star.png");
		fire.Load("./gfx/particles/fire.png");
		smoke.Load("./gfx/particles/smoke2.png");
		circle.Load("./gfx/particles/circle.png");


		img[0] = &diamond;
		img[1] = &star;
		img[2] = &fire;
		img[3] = &smoke;
		img[4] = &circle;

		particleEngine.Load();

		// Seed rand so it's not the same random numbers each time.
		std::srand(static_cast<unsigned int>(std::time(nullptr)));

		// Create 200 random particles
		vec2d start = { float(rand() % 156) + 50, float(rand() % 60) + 180 };
		
		for (int i = 0; i < 1; i++)
		{
			Particle p;
			p.position = start;
			p.velocity = { 40, -60 };
			float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float r2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			p.acceleration = { -r * 80 , r2 * 60};
			p.color = olc::Pixel(rand() % 256, rand() % 256, rand() % 256, rand() % 256);
			p.alive = true;
			p.scale = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) ;
			int whichImg = rand() % 5;
			p.sprite = img[whichImg];
			p.max_lifetime = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 5.0f;

			particleEngine.particles.push_back(p);
		}

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{	
		//SetPixelMode(olc::Pixel::ALPHA);
		//SetPixelMode(olc::Pixel::NORMAL);
		particleEngine.Update(this, fElapsedTime);
		particleEngine.Draw(this);

		std::cout << particleEngine.particles.size() << '\n';

		return true;
	}
};

int main()
{
	Example demo;
	if (demo.Construct(640, 480, 2, 2))
		demo.Start();
	return 0;
}