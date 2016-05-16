#ifndef ROGUELIKE_DUNGEON_GENERATOR_H
#define ROGUELIKE_DUNGEON_GENERATOR_H

#ifdef _DEBUG
#include <iostream>>
#endif // _DEBUG

#include <stack>
#include <queue>
#include <cstdlib>
#include <ctime>
using namespace std;


struct DungeonInfo {
	unsigned int width, height;
	unsigned int depth;
	bool** info;
	DungeonInfo() {
		width = height = depth = 0;
		info = nullptr;
	}
};


struct Point {
	unsigned int x, y;
	Point() {
		x = y = 0;
	}
	void Init(int X, int Y) {
		x = X;
		y = Y;
	}
};

struct Space {
	Point upperLeft, downRight;
	void Init(Point UpperLeft, Point DownRight) {
		upperLeft.Init(UpperLeft.x, UpperLeft.y);
		downRight.Init(DownRight.x, DownRight.y);
	}
};

struct BSPNode {
	bool isDivideInVertical;
	BSPNode *frontNode, *rearNode;
	Space space, room;
	BSPNode() {
		isDivideInVertical = 0;
		frontNode = rearNode = nullptr;
	}
	bool IsReafNode(void) { return (frontNode == nullptr && rearNode == nullptr); }
};

struct BSPTree {
	BSPNode* root;
	BSPTree() {
		root = nullptr;
	}
};


class RoguelikeDungeonGenerator {
public:
	RoguelikeDungeonGenerator(void);
	~RoguelikeDungeonGenerator(void);

	// Setters
	void SetWidth(unsigned int width);
	void SetHeight(unsigned int height);
	void SetDepth(unsigned int depth);

	void Build(void);
	DungeonInfo Publish(void);

protected:
	DungeonInfo dungeon;
	BSPTree tree;

	void Allocate(void);
	void Deallocate(void);

private:
	// Context methods used in Build
	void BuildBSPTree(void);
	void PathBuild(void);
	void RandomRoomBuild(BSPNode* node);
	void RandomPathBuild(BSPNode* node);

	// Methods used in RandomRoomBuild(BSPNode* node)
	void BuildRoom(BSPNode* node);
	bool RoomHasNotEnoughArea(BSPNode* node);
	void RecordRoomInfo(BSPNode* node);

	// Methods used in Setters
	bool WidthIsChanged(unsigned int width);
	bool HeightIsChanged(unsigned int height);
	bool DepthIsChanged(unsigned int depth);
	void DiscardLegacyDungeonInfo(void);

	// Methods used in Allocate(void) and Deallocate(void)
	void AllocateDungeonInfo(void);
	void AllocateBSPTree(void);
	void DeallocateDungeonInfo(void);
	void SearchAllNodesInBSPTree(vector<BSPNode*>* traversalList);
	void DeallocateAllNodesInBSPTree(vector<BSPNode*>* traversalList);

	// Methods used in BuildBSPTree(void)
	void SetRootOfBSPTree(void);
};


#endif