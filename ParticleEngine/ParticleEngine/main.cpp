#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <list>
#include <time.h>





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
	};

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
				|| position.y < 0)
			{
				alive = false;
			}


			if (alive)
			{
				// Update Velocity based on acceleration for this time slice (dElapsedTime)
				velocity.x = velocity.x + (acceleration.x * dElapsedTime);
				velocity.y = velocity.y + (acceleration.y * dElapsedTime);

				// Update position based on the current velocity in this given time slice (dElapsedTime)
				position.x = position.x + (velocity.x * dElapsedTime);
				position.y = position.y + (velocity.y * dElapsedTime);

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
		float max_lifetime = 0.0f;
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

		void Update(olc::PixelGameEngine* pge, float dElapsedTime)
		{
			for (Particle& particle : particles)
				particle.Update(pge, dElapsedTime);
		}

		void Draw(olc::PixelGameEngine* pge)
		{
			for (Particle& particle : particles)
				particle.Draw(pge);
				
		}

	public:
		std::vector<Particle> particles;

	};

	ParticleEngine particleEngine;
	Renderable blue;
	Renderable star;
	Renderable fire;
	Renderable smoke;
	Renderable circle;

public:
	bool OnUserCreate() override
	{
		// Load all particle images
		blue.Load("./gfx/particles/blue.png");
		star.Load("./gfx/particles/star.png");
		fire.Load("./gfx/particles/fire.png");
		smoke.Load("./gfx/particles/smoke2.png");
		circle.Load("./gfx/particles/darkbluecircle.png");

		Renderable* img[5];
		img[0] = &blue;
		img[1] = &star;
		img[2] = &fire;
		img[3] = &smoke;
		img[4] = &circle;

		// Seed rand so it's not the same random numbers each time.
		std::srand(static_cast<unsigned int>(std::time(nullptr)));

		// Create 200 random particles
		vec2d start = { float(rand() % 156) + 50, float(rand() % 60) + 180 };
		
		for (int i = 0; i < 50000; i++)
		{
			Particle p;
			p.position = start;
			p.velocity = { 40, -60 };
			float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float r2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			p.acceleration = { -r * 80 , r2 * 65};
			p.color = olc::Pixel(rand() % 256, rand() % 256, rand() % 256, rand() % 256);
			p.alive = true;
			p.scale = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) ;
			int whichImg = rand() % 5;
			p.sprite = img[whichImg];

			particleEngine.particles.push_back(p);
		}

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{	
		SetPixelMode(olc::Pixel::ALPHA);
		particleEngine.Update(this, fElapsedTime);
		particleEngine.Draw(this);

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