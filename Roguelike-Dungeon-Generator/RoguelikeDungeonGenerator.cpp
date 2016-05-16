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

	if (dungeon.width < 100)
		throw new exception("The width of a dungeon must be larger than 100.");
	if (dungeon.height < 100)
		throw new exception("The height of a dungeon must be larger than 100.");
	if (dungeon.width * dungeon.height < pow(2, dungeon.depth) * 30)
		throw new exception("The depth of a dungeon must be larger.");

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
	return (node->room.downRight.x - node->room.upperLeft.x) * (node->room.downRight.y - node->room.upperLeft.y) == 0;
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
void RoguelikeDungeonGenerator::RandomPathBuild(BSPNode* node) {
	Enter

	BSPNode* frontNode = node->frontNode;
	BSPNode* rearNode = node->rearNode;
	Point pivot;

	Log ("Select Pivot")
	pivot.x = (rearNode->room.downRight.x + frontNode->room.upperLeft.x) / 2;
	pivot.y = (rearNode->room.downRight.y + frontNode->room.upperLeft.y) / 2;

	Log ("Record On Dungeon Info")
	dungeon.info[pivot.x][pivot.y] = true;

<<<<<<< HEAD
	/* Not implemented yet. */
#ifdef _DEBUG
	LOG("Not implemented yet...");
#endif // _DEBUG
=======
	Log (pivot.x << ", " << pivot.y)
>>>>>>> BRANCH01

	Log ("Resize Room")
	node->room.upperLeft = frontNode->room.upperLeft;
	node->room.downRight = rearNode->room.downRight;

	if (node->isDivideInVertical) {

	}
	else {

	}

	Exit
}
// Buildd pathes between rooms.
void RoguelikeDungeonGenerator::PathBuild(void) {
	Enter

<<<<<<< HEAD
#ifdef _DEBUG
	LOG("Find Reaf Nodes...");
#endif // _DEBUG

	// find reaf nodes.
	queue<BSPNode*>* workQueue = new queue<BSPNode*>();
	workQueue->push(tree.root);
	while (true) {
		BSPNode* node = workQueue->front();

		if (node->IsReafNode())
			break;

		workQueue->push(node->frontNode);
		workQueue->push(node->rearNode);
	}

#ifdef _DEBUG
	LOG("Build Pathes...");
#endif // _DEBUG

	// from bottom to top, connect child nodes.
	while (workQueue->empty() == false) {
		BSPNode* node = workQueue->front();
		workQueue->pop();

		if (node->IsReafNode())
			continue;

		RandomPathBuild(node);
	}

#ifdef _DEBUG
	_LEVEL--;
#endif // _DEBUG
=======
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
>>>>>>> BRANCH01

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

	Log ("Not Implemented")

	Exit return dungeon;
}