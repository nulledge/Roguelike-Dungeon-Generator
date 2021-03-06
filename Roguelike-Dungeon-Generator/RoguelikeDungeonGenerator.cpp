#include "RoguelikeDungeonGenerator.h"

#ifdef _DEBUG
unsigned int _LEVEL = 0; // size of of indentation.
#define Enter \
	for (unsigned int i = 0; i < _LEVEL; i++) \
		cout << "	"; \
	cout << __func__ << "()" << endl;\
	_LEVEL += 1;

#define Log(x) \
	for (unsigned int i = 0; i < _LEVEL; i++) \
		cout << "	"; \
	cout << x << endl;

#define Exit \
	_LEVEL -= 1;

#else
#define Enter \
	do {} while(false);
#define Log(x) \
	do {} while(false);
#define Exit \
	do {} while(false);
#endif // _DEBUG


/*
	Not declared in header file.
*/
// Swap two unsigned 32 int variables.
void swap(unsigned int* a, unsigned int* b) {
	unsigned int temp = *a;
	*a = *b;
	*b = temp;
}
// Return random value  between 0 and b-a-1.
unsigned int rand(unsigned int a, unsigned int b) {
	if (a == b)
		return 0;
	if (a > b) {
		unsigned int temp = a;
		a = b;
		b = temp;
	}
	return rand() % (b - a);
}
unsigned int pow(unsigned int base, unsigned int exp) {
	unsigned int value = 1;
	for (unsigned int i = 0; i < exp; i++)
		value *= base;
	return value;
}


RoguelikeDungeonGenerator::RoguelikeDungeonGenerator(void) {
	Enter

	Log("Constructor do nothing...")

	Exit
}
RoguelikeDungeonGenerator::~RoguelikeDungeonGenerator(void) {
	Enter

	Deallocate();

	Exit
}

bool RoguelikeDungeonGenerator::WidthIsChanged(unsigned int width) {
	return dungeon.width != width;
}
bool RoguelikeDungeonGenerator::HeightIsChanged(unsigned int height) {
	return dungeon.height != height;
}
bool RoguelikeDungeonGenerator::DepthIsChanged(unsigned int depth) {
	return dungeon.depth != depth;
}
void RoguelikeDungeonGenerator::DiscardLegacyDungeonInfo() {
	Deallocate();
}
void RoguelikeDungeonGenerator::SetWidth(unsigned int width) {
	Enter

	if (WidthIsChanged(width))
		DiscardLegacyDungeonInfo();
	dungeon.width = width;

	Exit
}
void RoguelikeDungeonGenerator::SetHeight(unsigned int height) {
	Enter

	if (HeightIsChanged(height))
		DiscardLegacyDungeonInfo();
	dungeon.height = height;

	Exit
}
void RoguelikeDungeonGenerator::SetDepth(unsigned int depth) {
	Enter

	if (DepthIsChanged(depth))
		DiscardLegacyDungeonInfo();
	dungeon.depth = depth;

	Exit
}


void RoguelikeDungeonGenerator::AllocateDungeonInfo(void) {
	dungeon.info = new bool*[dungeon.width];
	for (unsigned int i = 0; i < dungeon.width; i++) {
		dungeon.info[i] = new bool[dungeon.height];
		for (unsigned int j = 0; j < dungeon.height; j++)
			dungeon.info[i][j] = false;
	}
}
void RoguelikeDungeonGenerator::AllocateBSPTree(void) {
	tree.root = new BSPNode;
}
void RoguelikeDungeonGenerator::Allocate(void) {
	Enter

	AllocateDungeonInfo();
	Log("dungeon info is allocated...")
	AllocateBSPTree();
	Log ("BSP tree is allocated...")
	Exit
}


void RoguelikeDungeonGenerator::DeallocateDungeonInfo(void) {
	for (unsigned int i = 0; i < dungeon.width; i++)
		delete[] dungeon.info[i];
	delete[] dungeon.info;
	dungeon.info = nullptr;
}
void RoguelikeDungeonGenerator::SearchAllNodesInBSPTree(vector<BSPNode*>* traversalList) {
	unsigned int index = 0;
	traversalList->push_back(tree.root);

	while (true) {
		BSPNode* node = traversalList->at(index);
		index += 1;

		if (node->IsReafNode())
			break;

		traversalList->push_back(node->frontNode);
		traversalList->push_back(node->rearNode);
	}
}
void RoguelikeDungeonGenerator::DeallocateAllNodesInBSPTree(vector<BSPNode*>* traversalList) {
	unsigned int index = (traversalList->empty() ? 0 : traversalList->size() - 1);
	while (traversalList->empty() == false) {
		BSPNode* node = traversalList->at(index);
		index -= 1;
		traversalList->pop_back();

		if (node->frontNode != nullptr)
			delete (node->frontNode);
		if (node->rearNode != nullptr)
			delete (node->rearNode);
	}
	delete tree.root;
	tree.root = nullptr;
}
void RoguelikeDungeonGenerator::Deallocate(void) {
	Enter

	if (dungeon.info != nullptr) {
		DeallocateDungeonInfo();
		Log("dungeon info is deallocated...")
	}

	if (tree.root != nullptr) {
		vector<BSPNode*>* traversalList = new vector<BSPNode*>;
		SearchAllNodesInBSPTree(traversalList);
		DeallocateAllNodesInBSPTree(traversalList);
		delete traversalList;
		Log ("BSP tree is deallocated...")
	}
	Exit
}


void RoguelikeDungeonGenerator::Build(void) {
	Enter

	srand(time(NULL));
	Allocate();
	BuildBSPTree();
	PathBuild();

	Exit
}

void RoguelikeDungeonGenerator::SetRootOfBSPTree(void) {
	Point upperLeft, downRight;
	upperLeft.Init(0, 0);
	downRight.Init(dungeon.width - 1, dungeon.height - 1);
	tree.root->space.Init(upperLeft, downRight);
}
void RoguelikeDungeonGenerator::BuildBSPTree(void) {
	Enter

	queue<BSPNode*>* workQueue;
	unsigned int count; // count is the number of rooms will be built.

	SetRootOfBSPTree();

	workQueue = new queue<BSPNode*>;
	workQueue->push(tree.root);
	count = pow(2, dungeon.depth) - 1;

	while (workQueue->empty() == false) {
		BSPNode* node = workQueue->front();
		workQueue->pop();

		Log ("Space Size is (" << node->space.upperLeft.x << ", " << node->space.upperLeft.y << ") ~ ("
			<< node->space.downRight.x << ", " << node->space.downRight.y << ")")

		// if dividing a dungeon is finished.
		if (count == 0) {
			Log("Reef Node....")

			// build a room.
			RandomRoomBuild(node);
		}
		// if dividing a dungeon is not finished.
		else {
			Log ("Not Reef Node...")

			node->frontNode = new BSPNode;
			node->frontNode->space.Init(node->space.upperLeft, node->space.downRight);
			node->rearNode = new BSPNode;
			node->rearNode->space.Init(node->space.upperLeft, node->space.downRight);

			Log("Child nodes are created...")

			// divide the given space into 2 pieces.
			// Space is divided in vertical if isDivideInVertical is true.
			// pivot is a reference point.
			unsigned int pivot;
			node->isDivideInVertical = (rand() % 2 == 1);
			if (node->isDivideInVertical) {
				pivot = node->space.upperLeft.x + (node->space.downRight.x - node->space.upperLeft.x) / 2 + 1;
				node->frontNode->space.downRight.x = node->rearNode->space.upperLeft.x = pivot;
			}
			else {
				pivot = node->space.upperLeft.y + (node->space.downRight.y - node->space.upperLeft.y) / 2 + 1;
				node->frontNode->space.downRight.y = node->rearNode->space.upperLeft.y = pivot;
			}

			Log ("	and divided into 2 areas...")

			// do same things to 2 divided pieces.
			workQueue->push(node->frontNode);
			workQueue->push(node->rearNode);
			count--;
		}
		Log (endl)
	}
	delete workQueue;
	Exit
}

void RoguelikeDungeonGenerator::BuildRoom(BSPNode* node) {
	node->room.upperLeft.x = node->space.upperLeft.x + rand(node->space.upperLeft.x, node->space.downRight.x);
	node->room.upperLeft.y = node->space.upperLeft.y + rand(node->space.upperLeft.y, node->space.downRight.y);
	node->room.downRight.x = node->room.upperLeft.x + rand(node->room.upperLeft.x, node->space.downRight.x);
	node->room.downRight.y = node->room.upperLeft.y + rand(node->room.upperLeft.y, node->space.downRight.y);
}
bool RoguelikeDungeonGenerator::RoomHasNotEnoughArea(BSPNode* node) {
	unsigned int roomWidth, roomHeight, roomArea;
	unsigned int spaceWidth, spaceHeight, spaceArea;

	roomWidth = (node->room.downRight.x - node->room.upperLeft.x);
	roomHeight = (node->room.downRight.y - node->room.upperLeft.y);
	roomArea = roomWidth * roomHeight;

	spaceWidth = (node->space.downRight.x - node->space.upperLeft.x);
	spaceHeight = (node->space.downRight.y - node->space.upperLeft.y);
	spaceArea = spaceWidth * spaceHeight;

	return (double)roomArea >= (double)spaceArea * 0.8f
		|| (double)roomArea <= (double)spaceArea * 0.5f;
}
void RoguelikeDungeonGenerator::RecordRoomInfo(BSPNode* node) {
	for (unsigned int i = node->room.upperLeft.x; i < node->room.downRight.x; i++)
		for (unsigned int j = node->room.upperLeft.y; j < node->room.downRight.y; j++)
			dungeon.info[i][j] = true;
}
void RoguelikeDungeonGenerator::RandomRoomBuild(BSPNode* node) {
	Enter

	do {
		BuildRoom(node);
	} while (RoomHasNotEnoughArea(node));
	RecordRoomInfo(node);

	Log("Room Size is (" << node->room.upperLeft.x << ", " << node->room.upperLeft.y << ") ~ ("
		<< node->room.downRight.x << ", " << node->room.downRight.y << ")")

	Exit
}
bool RoguelikeDungeonGenerator::PathNeedCorner(BSPNode* node) {
	BSPNode* frontNode = node->frontNode;
	BSPNode* rearNode = node->rearNode;

	if (node->isDivideInVertical) 
		return (frontNode->room.downRight.y <= rearNode->room.upperLeft.y
			|| frontNode->room.upperLeft.y >= rearNode->room.downRight.y) == false;
	else
		return (frontNode->room.downRight.x <= rearNode->room.upperLeft.x
			|| frontNode->room.upperLeft.x >= rearNode->room.downRight.x) == false;
}
void RoguelikeDungeonGenerator::SelectPivot(BSPNode* node, Point* pivot) {
	BSPNode* frontNode = node->frontNode;
	BSPNode* rearNode = node->rearNode;
	pivot->x = (rearNode->room.downRight.x + frontNode->room.upperLeft.x) / 2;
	pivot->y = (rearNode->room.downRight.y + frontNode->room.upperLeft.y) / 2;
}
void RoguelikeDungeonGenerator::UniteTwoRoomsIntoOneRoom(BSPNode* node) {
	BSPNode* frontNode = node->frontNode;
	BSPNode* rearNode = node->rearNode;
	node->room.upperLeft.Init(min(frontNode->room.upperLeft.x, rearNode->room.upperLeft.x),
		min(frontNode->room.upperLeft.y, rearNode->room.upperLeft.y));
	node->room.downRight.Init(max(frontNode->room.downRight.x, rearNode->room.downRight.x),
		max(frontNode->room.downRight.y, rearNode->room.downRight.y));
}
void RoguelikeDungeonGenerator::RecordPathInfo(BSPNode* node, Point* start, Point* end) {
	unsigned int x, y;
	if (start->x == end->x) {
		x = start->x;
		for (y = start->y; y < end->y; y++)
			dungeon.info[x][y] = true;
		for (y = end->y; y < dungeon.height; y++) {
			if (dungeon.info[x][y])
				break;
			dungeon.info[x][y] = true;
		}
		for (y = start->y - 1; y >= 0; y--) {
			if (dungeon.info[x][y])
				break;
			dungeon.info[x][y] = true;
		}
	}
	else {
		y = start->y;
		for (x = start->x; x < end->x; x++)
			dungeon.info[x][y] = true;
		for (x = end->x; x < dungeon.width; x++) {
			if (dungeon.info[x][y])
				break;
			dungeon.info[x][y] = true;
		}
		for (x = start->x - 1; x >= 0; x--) {
			if (dungeon.info[x][y])
				break;
			dungeon.info[x][y] = true;
		}
	}
}
void RoguelikeDungeonGenerator::RandomPathBuild(BSPNode* node) {
	Enter

	BSPNode* frontNode = node->frontNode;
	BSPNode* rearNode = node->rearNode;
	Point* pivot = new Point;
	Point* start = new Point;
	Point* end = new Point;

	SelectPivot(node, pivot);
	UniteTwoRoomsIntoOneRoom(node);
	if (node->isDivideInVertical) {
		start->x = frontNode->room.downRight.x;
		end->x = rearNode->room.upperLeft.x;
		start->y = end->y = pivot->y;
		RecordPathInfo(node, start, end);
	}
	else {
		start->y = frontNode->room.downRight.y;
		end->y = rearNode->room.upperLeft.y;
		start->x = end->x = pivot->x;
		RecordPathInfo(node, start, end);
	}

	delete pivot;
	delete start;
	delete end;

	Exit
}
// Buildd pathes between rooms.
void RoguelikeDungeonGenerator::PathBuild(void) {
	Enter

	// traversalDeque is for search all nodes.
	vector<BSPNode*>* nodes = new vector<BSPNode*>;
	size_t index = 0;
	nodes->push_back (tree.root);

	Log ("Find All Nodes...")
	while (true) {
		BSPNode* node = nodes->at(index);
		index += 1;

		if (node->IsReafNode())
			break;
		nodes->push_back(node->frontNode);
		nodes->push_back(node->rearNode);
	}

	// Build pathes between rooms. From reef node to root node.
	Log("Build Pathes Between Rooms...")
	while (nodes->empty() == false) {
		BSPNode* node = nodes->at(nodes->size() - 1);
		nodes->pop_back();

		// a reaf node has no child room to connect.
		if (node->IsReafNode())
			continue;

		RandomPathBuild (node);
	}
	delete nodes;
	Exit
}


/*
	Publish the dungeon info.
	TODO: dungeon.info is a pointer so the reference is copies. Fix it not to be copied.
*/
DungeonInfo RoguelikeDungeonGenerator::Publish(void) {
	Enter

	DungeonInfo ReturnValue;

	ReturnValue.width = dungeon.width;
	ReturnValue.height = dungeon.height;
	ReturnValue.depth = dungeon.depth;
	ReturnValue.info = new bool*[ReturnValue.width];
	for (unsigned int i = 0; i < ReturnValue.width; i++) {
		ReturnValue.info[i] = new bool[ReturnValue.height];
		for (unsigned int j = 0; j < ReturnValue.height; j++)
			ReturnValue.info[i][j] = dungeon.info[i][j];
	}

	Exit return ReturnValue;
}