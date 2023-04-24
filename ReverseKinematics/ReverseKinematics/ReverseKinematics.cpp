#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class ReverseKinematics : public olc::PixelGameEngine
{
public:
	ReverseKinematics()
	{
		sAppName = "ReverseKinematics";
	}

	// Simple struct to make accessing X/Y easier.
	// Could expand this out so we have a "constructor" so we don't have to call .x and .y to just set them
	struct Vec2d
	{
		float x, y;
	};

	// Overkill for now, but maybe we'll expand on what attributers we have for a node later
	struct Node
	{
		Vec2d position;
	};

	struct Link
	{
		Node* nodeA;
		Node* nodeB;
	};

	// Objects
	Node* nodes;
	Link* links;

	int maxNodes = 3;
	int maxLinks = 2;

	int selectedNode = -1;

public:
	bool OnUserCreate() override
	{
		// Create memory/space for 3 nodes, with 2 lines between them
		nodes = new Node[maxNodes];
		links = new Link[maxLinks];

		// Populate the nodes and links with starting data
		nodes[0].position.x = 30.0;
		nodes[0].position.y = 30.0;

		nodes[1].position.x = 50.0;
		nodes[1].position.y = 70.0;

		nodes[2].position.x = 60.0;
		nodes[2].position.y = 100.0;

		links[0].nodeA = &nodes[0];
		links[0].nodeB = &nodes[1];

		links[1].nodeA = &nodes[1];
		links[1].nodeB = &nodes[2];

		return true;
	}

	int CheckForClickedNode(int x, int y)
	{
		// How many pixels to the LEFT and RIGHT of the node do we consider "inside"
		int selectabeRadius = 3; // Could move this to global scope as a "property"

		// we'll just do a nieve approach, in a more performant situation, you'd want to do a quad tree perhaps.
		for (int n = 0; n < maxNodes; n++)
		{
			if (nodes[n].position.x - 3 <= x
				&& nodes[n].position.y - 3 <= y
				&& nodes[n].position.x + 3 >= x
				&& nodes[n].position.y + 3 >= y
				)
			{
				return n;
			}
		}

		return -1;
	}

	void DrawNodesAndLinks()
	{
		// loop over and draw all nodes
		for (int n = 0; n < maxNodes; n++)
		{
			DrawCircle(nodes[n].position.x, nodes[n].position.y, 10, olc::Pixel(255, 255, 255));
			// 	Draw(nodes[n].position.x, nodes[n].position.y, olc::Pixel(255, 0, 0));
		}

		// loop over and draw all links
		for (int l = 0; l < maxLinks; l++)
		{
			olc::vi2d startPos; // could also use this to replace out Vec2d
			olc::vi2d endPos;
			startPos.x = links[l].nodeA->position.x;
			startPos.y = links[l].nodeA->position.y;
			endPos.x = links[l].nodeB->position.x;
			endPos.y = links[l].nodeB->position.y;

			DrawLine(startPos, endPos, olc::Pixel(255, 255, 255));
		}
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLACK);

		olc::vi2d mousePosition = GetMousePos();
		if (GetMouse(0).bHeld && selectedNode == -1)
		{
			selectedNode = CheckForClickedNode(mousePosition.x, mousePosition.y);
			std::cout << selectedNode << " " << mousePosition << "\n";
		}

		if (GetMouse(0).bReleased)
			selectedNode = -1;


		if (selectedNode != -1)
		{
			// Move selected node
			nodes[selectedNode].position.x = mousePosition.x;
			nodes[selectedNode].position.y = mousePosition.y;
			
			// check for connections for this node, and add their 
		}

		DrawNodesAndLinks();
		Draw(mousePosition, olc::Pixel(255, 0, 0));
		
		return true;
	}
};


int main()
{
	ReverseKinematics demo;
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();

	return 0;
}