#include "RoguelikeDungeonGenerator.h"

#ifdef _DEBUG
unsigned int _LEVEL = 0;
void LOG() {
	for (unsigned int i = 0; i < _LEVEL; i++)
		cout << "	";
}
void LOG(char* message) {
	for (unsigned int i = 0; i < _LEVEL; i++)
		cout << "	";
	cout << message << endl;
}
#endif // _DEBUG


/*
	Not declared in header file.
*/
void swap(unsigned int* a, unsigned int* b) {
	unsigned int temp = *a;
	*a = *b;
	*b = temp;
}
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
unsigned int pow(int base, unsigned int exp) {
	unsigned int value = 1;
	for (unsigned int i = 0; i < exp; i++)
		value *= base;
	return value;
}


/*
	Basic Constructor and Destructor
*/
RoguelikeDungeonGenerator::RoguelikeDungeonGenerator(void) {
#ifdef _DEBUG
	LOG("Generator.Constructor()");
	_LEVEL++;
#endif // _DEBUG

	dungeon.width = 0;
	dungeon.height = 0;
	dungeon.info = nullptr;

	tree.root = nullptr;

#ifdef _DEBUG
	_LEVEL--;
#endif // _DEBUG
}
RoguelikeDungeonGenerator::~RoguelikeDungeonGenerator(void) {
#ifdef _DEBUG
	LOG("Generator.Destructor()");
	_LEVEL++;
#endif // _DEBUG

	Deallocate();

#ifdef _DEBUG
	_LEVEL--;
#endif // _DEBUG
}


/*
	Set Dungeon Info
*/
void RoguelikeDungeonGenerator::SetWidth(unsigned int width) {
#ifdef _DEBUG
	LOG("Generator.SetWidth()");
	_LEVEL++;
#endif // _DEBUG

	if (dungeon.width != width)
		Deallocate();
	dungeon.width = width;

#ifdef _DEBUG
	_LEVEL--;
#endif // _DEBUG
}
void RoguelikeDungeonGenerator::SetHeight(unsigned int height) {
#ifdef _DEBUG
	LOG("Generator.SetHeight()");
	_LEVEL++;
#endif // _DEBUG

	if (dungeon.height != height)
		Deallocate();
	dungeon.height = height;

#ifdef _DEBUG
	_LEVEL--;
#endif // _DEBUG
}
void RoguelikeDungeonGenerator::SetDepth(unsigned int depth) {
#ifdef _DEBUG
	LOG("Generator.SetDepth()");
	_LEVEL++;
#endif // _DEBUG

	if (dungeon.depth != depth)
		Deallocate();
	dungeon.depth = depth;

#ifdef _DEBUG
	_LEVEL--;
#endif // _DEBUG
}
void RoguelikeDungeonGenerator::Allocate(void) {
#ifdef _DEBUG
	LOG(".Allocate(void)");
	_LEVEL++;
#endif // _DEBUG

	dungeon.info = new bool*[dungeon.width];
	for (unsigned int i = 0; i < dungeon.width; i++) {
		dungeon.info[i] = new bool[dungeon.height];
		for (unsigned int j = 0; j < dungeon.height; j++)
			dungeon.info[i][j] = false;
	}

#ifdef _DEBUG
	LOG("dungeon info is allocated...");
#endif // _DEBUG

	tree.root = new BSPNode;
	tree.root->Init();

#ifdef _DEBUG
	LOG("BSP tree is allocated...");
	_LEVEL--;
#endif // _DEBUG

}
// Deallocate all references if needed.
void RoguelikeDungeonGenerator::Deallocate(void) {
#ifdef _DEBUG
	LOG(".Deallocate(void)");
	_LEVEL++;
#endif // _DEBUG

	// Deallocate the references in dungeon info.
	if (dungeon.info != nullptr) {
		for (unsigned int i = 0; i < dungeon.width; i++) {
			delete[] dungeon.info[i];
		}
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

			BSPNode *frontNode, *rearNode;
			frontNode = node->frontNode;
			rearNode = node->rearNode;

			if (frontNode == nullptr || rearNode == nullptr)
				break;
			
			traversalStack->push(frontNode);
			traversalStack->push(rearNode);
		}
		while (traversalStack->empty() == false) {
			BSPNode* node = traversalStack->top();
			traversalStack->pop();

			BSPNode *frontNode, *rearNode;
			frontNode = node->frontNode;
			rearNode = node->rearNode;

			if (frontNode != nullptr)
				delete frontNode;
			if (rearNode != nullptr)
				delete rearNode;
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
void RoguelikeDungeonGenerator::Build(void) {
#ifdef _DEBUG
	LOG("Generator.Build()");
	_LEVEL++;
#endif // _DEBUG

	srand(time(NULL));
	Allocate();
	BSP();

#ifdef _DEBUG
	_LEVEL--;
#endif // _DEBUG
}
void RoguelikeDungeonGenerator::BSP(void) {
#ifdef _DEBUG
	LOG(".BSP(void)");
	_LEVEL++;
#endif // _DEBUG

	queue<BSPNode*>* workQueue;
	unsigned int count;

	{
		Point upperLeft, downRight;
		upperLeft.Init(0, 0);
		downRight.Init(dungeon.width - 1, dungeon.height - 1);
		tree.root->space.Init(upperLeft, downRight);
	}

	workQueue = new queue<BSPNode*>;
	workQueue->push(tree.root);
	count = pow(2, dungeon.depth) - 1;

	while (workQueue->empty() == false) {
#ifdef _DEBUG
		cout << endl;
#endif // _DEBUG

		BSPNode* node = workQueue->front();
		workQueue->pop();

#ifdef _DEBUG
		LOG();
		cout << "Space Size is (" << node->space.upperLeft.x << ", " << node->space.upperLeft.y << ") ~ ("
			<< node->space.downRight.x << ", " << node->space.downRight.y << ")" << endl;
#endif // _DEBUG

		if (count == 0) {
#ifdef _DEBUG
			LOG("Reef Node....");
#endif // _DEUB

			RandomRoomBuild(node);
		}
		else {
#ifdef _DEBUG
			LOG("Not Reef Node...");
#endif // _DEBUG

			node->frontNode = new BSPNode;
			node->frontNode->Init(node->space.upperLeft, node->space.downRight);
			node->rearNode = new BSPNode;
			node->rearNode->Init(node->space.upperLeft, node->space.downRight);

#ifdef _DEBUG
			LOG("Child nodes are created...");
#endif // _DEBUG

			unsigned int pivot;
			node->isDivideInVertical = (rand() % 2 == 1);
			if (node->isDivideInVertical) {
				pivot = node->space.upperLeft.x + (node->space.downRight.x - node->space.upperLeft.x) / 2.0f + 1;
				node->frontNode->space.downRight.x = node->rearNode->space.upperLeft.x = pivot;
			}
			else {
				pivot = node->space.upperLeft.y + (node->space.downRight.y - node->space.upperLeft.y) / 2.0f + 1;
				node->frontNode->space.downRight.y = node->rearNode->space.upperLeft.y = pivot;
			}

#ifdef _DEBUG
			LOG("	and divided into 2 areas...");
#endif // _DEBUG

			workQueue->push(node->frontNode);
			workQueue->push(node->rearNode);
			count--;
		}
	}
	
	delete workQueue;

#ifdef _DEBUG
	_LEVEL--;
#endif // _DEBUG

}
void RoguelikeDungeonGenerator::RandomRoomBuild(BSPNode* node) {
#ifdef _DEBUG
	LOG(".RandomRoomBuild(BSPNode* node)");
	_LEVEL++;
#endif // _DEBUG

	do {
		node->room.upperLeft.x = node->space.upperLeft.x + rand(node->space.downRight.x, node->space.upperLeft.x);
		node->room.upperLeft.y = node->space.upperLeft.y + rand(node->space.downRight.y, node->space.upperLeft.y);
		node->room.downRight.x = node->space.upperLeft.x + rand(node->space.downRight.x, node->space.upperLeft.x);
		node->room.downRight.y = node->space.upperLeft.y + rand(node->space.downRight.y, node->space.upperLeft.y);

		if (node->room.upperLeft.x > node->room.downRight.x)
			swap(&(node->room.upperLeft.x), &(node->room.downRight.x));
		if (node->room.upperLeft.y > node->room.downRight.y)
			swap(&(node->room.upperLeft.y), &(node->room.downRight.y));

	} while ( (node->room.downRight.x - node->room.upperLeft.x)
		* (node->room.downRight.y - node->room.upperLeft.y)
		>= (node->space.downRight.x - node->space.upperLeft.x)
			* (node->space.downRight.y - node->space.upperLeft.y) / 2);

#ifdef _DEBUG
	LOG();
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


/*
	Publish the dungeon info
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