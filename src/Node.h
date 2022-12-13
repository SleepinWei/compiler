#pragma once
#include"component.h"
class GrammarEntry;

class Node {
public:
	Node() { left = right = nullptr; code = place = op = ""; }
	~Node() {};

public:
	Node* left;
	Node* right;

	string code;
	string place;
	string op;
	//GrammarEntry* entry;
};
