#pragma once
#include"component.h"
class GrammarEntry;

class Node {
public:
	Node() {};
	~Node() {};

public:
	std::vector<Node*> children;

	string code;
	string place;
	bool isTerminal;
	//GrammarEntry* entry;
};
