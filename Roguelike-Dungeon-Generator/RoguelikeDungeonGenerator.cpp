#include "RoguelikeDungeonGenerator.h"

#ifdef _DEBUG
unsigned int _LEVEL = 0; // size of of indentation.
// Log endline.
void LOGENDL() {
	for (unsigned int i = 0; i < _LEVEL; i++)
		cout << "	";
}
// Log message with endline.
void LOG(char* message) {
	for (unsigned int i = 0; i < _LEVEL; i++)
		cout << "	";
	cout << message << endl;
}
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
/*
	Power with integer base and exponential.
	Exponential must be non-negative.
*/
unsigned int pow(int base, unsigned int exp) {
	unsigned int value = 1;
	for (unsigned int i = 0; i < exp; i++)
		value *= base;
	return value;
}


/*
	Basic Constructor and Destructor
*/
// Default constructor.
RoguelikeDungeonGenerator::RoguelikeDungeonGenerator(void) {
#ifdef _DEBUG
	LOG("Generator.Constructor()");
	_LEVEL++;
#endif // _DEBUG

#ifdef _DEBUG
	LOG("Constructor do nothing...");
#endif // _DEBUG

#ifdef _DEBUG
	_LEVEL--;
#endif // _DEBUG
}
// Destructor.
RoguelikeDungeonGenerator::~RoguelikeDungeonGenerator(void) {
#ifdef _DEBUG
	LOG("Generator.Destructor()");
	_LEVEL++;
#endif // _DEBUG

	// deallocate all refereneces used.
	Deallocate();

#ifdef _DEBUG
	_LEVEL--;
#endif // _DEBUG
}


/*
	Set Dungeon Info
*/
// Set the width of a dungeon.
void RoguelikeDungeonGenerator::SetWidth(unsigned int width) {
#ifdef _DEBUG
	LOG("Generator.SetWidth()");
	_LEVEL++;
#endif // _DEBUG

	// if the width of a dungeon is changed then delete previous dungeon info.
	if (dungeon.width != width)
		Deallocate();
	dungeon.width = width;

#ifdef _DEBUG
	_LEVEL--;
#endif // _DEBUG
}
// Set the height of a dungeon.
void RoguelikeDungeonGenerator::SetHeight(unsigned int height) {
#ifdef _DEBUG
	LOG("Generator.SetHeight()");
	_LEVEL++;
#endif // _DEBUG

	// if the height of a dungeon is changed then delete previous dungeon info.
	if (dungeon.height != height)
		Deallocate();
	dungeon.height = height;

#ifdef _DEBUG
	_LEVEL--;
#endif // _DEBUG
}
// Set the depth of a dungeon.
// A dungeon has 2^depth rooms.
void RoguelikeDungeonGenerator::SetDepth(unsigned int depth) {
#ifdef _DEBUG
	LOG("Generator.SetDepth()");
	_LEVEL++;
#endif // _DEBUG

	// if the depth of a dungeon is changed then delete previous dungeon info.
	if (dungeon.depth != depth)
		Deallocate();
	dungeon.depth = depth;

#ifdef _DEBUG
	_LEVEL--;
#endif // _DEBUG
}
// Allocate a dungeon info and a BSP tree.
void RoguelikeDungeonGenerator::Allocate(void) {
#ifdef _DEBUG
	LOG(".Allocate(void)");
	_LEVEL++;
#endif // _DEBUG

	// allocate and initialize a dungeon info.
	dungeon.info = new bool*[dungeon.width];
	for (unsigned int i = 0; i < dungeon.width; i++) {
		dungeon.info[i] = new bool[dungeon.height];
		for (unsigned int j = 0; j < dungeon.height; j++)
			dungeon.info[i][j] = false;
	}

#ifdef _DEBUG
	LOG("dungeon info is allocated...");
#endif // _DEBUG

	// allocate and initialize BSP tree.
	tree.root = new BSPNode;

#ifdef _DEBUG
	LOG("BSP tree is allocated...");
	_LEVEL--;
#endif // _DEBUG

}
// Deallocate all references.
void RoguelikeDungeonGenerator::Deallocate(void) {
#ifdef _DEBUG
	LOG(".Deallocate(void)");
	_LEVEL++;
#endif // _DEBUG

	// Deallocate the references in dungeon info.
	if (dungeon.info != nullptr) {
		for (unsigned int i = 0; i < dungeon.width; i++)
			delete[] dungeon.info[i];
		delete[] dungeon.info;
		dungeon.info = nullptr;

#ifdef _DEBUG
		LOG("dungeon info is deallocated...");
#endif // _DEBUG
	}

	// Deallocate the references in BSP tree.
	if (tree.root != nullptr) {
		stack<BSPNode*>* traversalStack = new stack<BSPNode*>;
		traversalStack->push(tree.root);

		while (true) {
			BSPNode* node = traversalStack->top();

			if (node->IsReafNode())
				break;
			
			traversalStack->push(node->frontNode);
			traversalStack->push(node->rearNode);
		}
		while (traversalStack->empty() == false) {
			BSPNode* node = traversalStack->top();
			traversalStack->pop();

			if (node->frontNode != nullptr)
				delete (node->frontNode);
			if (node->rearNode != nullptr)
				delete (node->rearNode);
		}
		delete tree.root;
		tree.root = nullptr;

		delete traversalStack;

#ifdef _DEBUG
		LOG("BSP tree is deallocated...");
#endif // _DEBUG

	}

#ifdef _DEBUG
	_LEVEL--;
#endif // _DEBUG
}

/*
	Build Dungeon
*/
// Build a dungeon.
void RoguelikeDungeonGenerator::Build(void) {
#ifdef _DEBUG
	LOG("Generator.Build()");
	_LEVEL++;
#endif // _DEBUG

	if (dungeon.width < 100)
		throw new exception("The width of a dungeon must be larger than 100.");
	if (dungeon.height < 100)
		throw new exception("The height of a dungeon must be larger than 100.");
	if (dungeon.width * dungeon.height < pow(2, dungeon.depth) * 30)
		throw new exception("The depth of a dungeon must be larger.");

	srand(time(NULL));
	Allocate();
	BSP();
	PathBuild();

#ifdef _DEBUG
	_LEVEL--;
#endif // _DEBUG
}
// Seperate a dungeon into pieces using BSP.
void RoguelikeDungeonGenerator::BSP(void) {
#ifdef _DEBUG
	LOG(".BSP(void)");
	_LEVEL++;
#endif // _DEBUG

	queue<BSPNode*>* workQueue;
	unsigned int count;

	// set the root of the BSP tree.
	{
		Point upperLeft, downRight;
		upperLeft.Init(0, 0);
		downRight.Init(dungeon.width - 1, dungeon.height - 1);
		tree.root->space.Init(upperLeft, downRight);
	}

	workQueue = new queue<BSPNode*>;
	workQueue->push(tree.root);
	count = pow(2, dungeon.depth) - 1; // count is the number of rooms will be built.

	while (workQueue->empty() == false) {
		BSPNode* node = workQueue->front();
		workQueue->pop();

#ifdef _DEBUG
		LOGENDL();
		cout << "Space Size is (" << node->space.upperLeft.x << ", " << node->space.upperLeft.y << ") ~ ("
			<< node->space.downRight.x << ", " << node->space.downRight.y << ")" << endl;
#endif // _DEBUG

		// if dividing a dungeon is finished.
		if (count == 0) {
#ifdef _DEBUG
			LOG("Reef Node....");
#endif // _DEUB

			// build a room.
			RandomRoomBuild(node);
		}
		// if dividing a dungeon is not finished.
		else {
#ifdef _DEBUG
			LOG("Not Reef Node...");
#endif // _DEBUG

			node->frontNode = new BSPNode;
			node->frontNode->space.Init(node->space.upperLeft, node->space.downRight);
			node->rearNode = new BSPNode;
			node->rearNode->space.Init(node->space.upperLeft, node->space.downRight);

#ifdef _DEBUG
			LOG("Child nodes are created...");
#endif // _DEBUG

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

#ifdef _DEBUG
			LOG("	and divided into 2 areas...");
#endif // _DEBUG

			// do same things to 2 divided pieces.
			workQueue->push(node->frontNode);
			workQueue->push(node->rearNode);
			count--;

#ifdef _DEBUG
			if (count) cout << endl;
#endif // _DEBUG

		}
	}
	
	delete workQueue;

#ifdef _DEBUG
	_LEVEL--;
#endif // _DEBUG

}
// Build a room in the given space.
void RoguelikeDungeonGenerator::RandomRoomBuild(BSPNode* node) {
#ifdef _DEBUG
	LOG(".RandomRoomBuild(BSPNode* node)");
	_LEVEL++;
#endif // _DEBUG

	do {
		node->room.upperLeft.x = node->space.upperLeft.x + rand(node->space.downRight.x, node->space.upperLeft.x) / 3;
		node->room.upperLeft.y = node->space.upperLeft.y + rand(node->space.downRight.y, node->space.upperLeft.y) / 3;
		node->room.downRight.x = node->room.upperLeft.x + rand(node->space.downRight.x, node->space.upperLeft.x);
		node->room.downRight.y = node->room.upperLeft.y + rand(node->space.downRight.y, node->space.upperLeft.y);

/*		if (node->room.upperLeft.x > node->room.downRight.x)
			swap(&(node->room.upperLeft.x), &(node->room.downRight.x));
		if (node->room.upperLeft.y > node->room.downRight.y)
			swap(&(node->room.upperLeft.y), &(node->room.downRight.y));*/

	} while ( node->room.downRight.x >= node->space.downRight.x
		|| node->room.downRight.y >= node->space.downRight.y);

#ifdef _DEBUG
	LOGENDL();
	cout << "Room Size is (" << node->room.upperLeft.x << ", " << node->room.upperLeft.y << ") ~ ("
		<< node->room.downRight.x << ", " << node->room.downRight.y << ")" << endl;
#endif // _DEBUG

	for (unsigned int i = node->room.upperLeft.x; i < node->room.downRight.x; i++)
		for (unsigned int j = node->room.upperLeft.y; j < node->room.downRight.y; j++)
			dungeon.info[i][j] = true;

#ifdef _DEBUG
	_LEVEL--;
#endif // _DEBUG
}
// Build a path connecting two children of node.
void RoguelikeDungeonGenerator::RandomPathBuild(BSPNode* node) {
#ifdef _DEBUG
	LOG(".RandomPathBuild(BSPNode* node)");
	_LEVEL++;
#endif // _DEBUG

	/* Not implemented yet. */
#ifdef _DEBUG
	LOG("Not Implemented yet...");
#endif // _DEBUG


#ifdef _DEBUG
	_LEVEL--;
#endif // _DEBUG
}
// Buildd pathes between rooms.
void RoguelikeDungeonGenerator::PathBuild(void) {
#ifdef _DEBUG
	LOG(".PathBuild(void)");
	_LEVEL++;
#endif // _DEBUG

	/* Not implemented. */
#ifdef _DEBUG
	LOG("Not implemented...");
#endif // _DEBUG

#ifdef _DEBUG
	_LEVEL--;
#endif // _DEBUG

}


/*
	Publish the dungeon info.
	TODO: dungeon.info is a pointer so the reference is copies. Fix it not to be copied.
*/
DungeonInfo RoguelikeDungeonGenerator::Publish(void) {
#ifdef _DEBUG
	LOG("Generator.Publish(void)");
	_LEVEL++;
#endif // _DEBUG

#ifdef _DEBUG
	_LEVEL--;
#endif // _DEBUG

	return dungeon;
}