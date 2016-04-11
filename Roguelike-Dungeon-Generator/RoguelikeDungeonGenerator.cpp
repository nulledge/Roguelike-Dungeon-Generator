#include "RoguelikeDungeonGenerator.h"

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
	dungeon.width = 0;
	dungeon.height = 0;
	dungeon.info = nullptr;
}
RoguelikeDungeonGenerator::~RoguelikeDungeonGenerator(void) {
	Deallocate();
}


/*
	Set Dungeon Info
*/
void RoguelikeDungeonGenerator::SetWidth(unsigned int width) {
	if (dungeon.width != width)
		Deallocate();
	dungeon.width = width;
}
void RoguelikeDungeonGenerator::SetHeight(unsigned int height) {
	if (dungeon.height != height)
		Deallocate();
	dungeon.height = height;
}
void RoguelikeDungeonGenerator::SetDepth(unsigned int depth) {
	if (dungeon.depth != depth)
		Deallocate();
	dungeon.depth = depth;
}
void RoguelikeDungeonGenerator::Allocate(void) {
	dungeon.info = new bool*[dungeon.width];
	for (unsigned int i = 0; i < dungeon.width; i++) {
		dungeon.info[i] = new bool[dungeon.height];
		for (unsigned int j = 0; j < dungeon.height; j++)
			dungeon.info[i][j] = false;
	}
}
void RoguelikeDungeonGenerator::Deallocate(void) {
	if (dungeon.info == nullptr)
		return;

	for (unsigned int i = 0; i < dungeon.width; i++) {
		delete[] dungeon.info[i];
	}
	delete[] dungeon.info;
	dungeon.info = nullptr;
}

/*
	Build Dungeon
*/
void RoguelikeDungeonGenerator::Build(void) {
	Allocate();
	BSP();
}
void RoguelikeDungeonGenerator::BSP(void) {
	queue<pair<Point, Point> >* workQueue;
	Point upperLeft, downRight;
	unsigned int count;

	srand(time(NULL));

	upperLeft.x = 0;
	upperLeft.y = 0;
	downRight.x = dungeon.width - 1;
	downRight.y = dungeon.height - 1;

	workQueue = new queue<pair<Point, Point> >;
	workQueue->push(make_pair(upperLeft, downRight));

	count = pow(2, dungeon.depth) - 1;

	while (workQueue->empty() == false && count != 0) {
		upperLeft = workQueue->front().first;
		downRight = workQueue->front().second;
		workQueue->pop();
		count--;

		Point pivot;
		bool divideInVertical = (rand() % 2 == 1);
		if (divideInVertical) {
			pivot.x = upperLeft.x + rand() % (downRight.x - upperLeft.x);
			pivot.y = downRight.y;

			workQueue->push(make_pair(upperLeft, pivot));
			pivot.y = upperLeft.y;
			workQueue->push(make_pair(pivot, downRight));
		}
		else {
			pivot.x = downRight.x;
			pivot.y = upperLeft.y + rand() % (downRight.y - upperLeft.y);

			workQueue->push(make_pair(upperLeft, pivot));
			pivot.x = upperLeft.x;
			workQueue->push(make_pair(pivot, downRight));
		}
	}

	while (workQueue->empty() == false) {
		upperLeft = workQueue->front().first;
		downRight = workQueue->front().second;
		workQueue->pop();

		RandomRoomBuild(upperLeft, downRight);
	}

	delete workQueue;
}
void RoguelikeDungeonGenerator::RandomRoomBuild(Point upperLeft, Point downRight) {
	Point newUpperLeft, newDownRight;
	newUpperLeft.x = upperLeft.x + rand(downRight.x, upperLeft.x);
	newUpperLeft.y = upperLeft.y + rand(downRight.y, upperLeft.y);
	newDownRight.x = upperLeft.x + rand(downRight.x, upperLeft.x);
	newDownRight.y = upperLeft.y + rand(downRight.y, upperLeft.y);

	if (newUpperLeft.x > newDownRight.x)
		swap(&(newUpperLeft.x), &(newDownRight.x));
	if (newUpperLeft.y > newDownRight.y)
		swap(&(newUpperLeft.y), &(newDownRight.y));

	for (unsigned int i = newUpperLeft.x; i < newDownRight.x; i++)
		for (unsigned int j = newUpperLeft.y; j < newDownRight.y; j++)
			dungeon.info[i][j] = true;
}
void RoguelikeDungeonGenerator::RandomPathBuild(void) {
	/* Not implemented yet. */
}


/*
	Publish the dungeon info
*/
DungeonInfo RoguelikeDungeonGenerator::Publish(void) {
	return dungeon;
}