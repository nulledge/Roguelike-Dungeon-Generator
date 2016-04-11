#ifndef ROGUELIKE_DUNGEON_GENERATOR_H
#define ROGUELIKE_DUNGEON_GENERATOR_H

#include <queue>
#include <cstdlib>
#include <ctime>
using namespace std;


struct DungeonInfo {
	unsigned int width, height;
	unsigned int depth;
	bool** info;
};


struct Point {
	unsigned int x, y;
};


class RoguelikeDungeonGenerator {
public:
	// Basic Constructor and Destructor
	RoguelikeDungeonGenerator(void);
	~RoguelikeDungeonGenerator(void);

	void SetWidth(unsigned int width);
	void SetHeight(unsigned int height);
	void SetDepth(unsigned int depth);

	void Build(void);
	DungeonInfo Publish(void);

private:
	DungeonInfo dungeon;

	void BSP(void);
	void RandomRoomBuild(Point upperLeft, Point downRight);
	void RandomPathBuild(void);

	void Allocate(void);
	void Deallocate(void);
};


#endif