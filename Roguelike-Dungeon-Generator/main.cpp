#include <iostream>
#include <fstream>
using namespace std;

#include "RoguelikeDungeonGenerator.h"

RoguelikeDungeonGenerator* generator = new RoguelikeDungeonGenerator();

int main(void) {
	unsigned int width, height, depth;
	ofstream output;

	cout << "Enter width :";
	cin >> width;

	cout << "Enter height :";
	cin >> height;

	cout << "Enter depth :";
	cin >> depth;

	generator->SetWidth(width);
	generator->SetHeight(height);
	generator->SetDepth(depth);

	generator->Build();

	DungeonInfo data = generator->Publish();

	output.open("output.txt");
	for (unsigned int i = 0; i < data.width; i++) {
		for (unsigned int j = 0; j < data.height; j++) {
			output << (data.info[i][j] ? "бс" : "бр");
		}
		output << endl;
	}
	output.close();
	delete generator;

	cout << "Press enter to exit...";
	cin.ignore();
	cin.get();

	return 0;
}