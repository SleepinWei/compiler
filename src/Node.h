#pragma once
#include"component.h"
class GrammarEntry;

class Node {
public:
	Node() { isTerminal = false; };
	Node(string type_, string place_,bool isTerminal_):type(type_),place(place_),isTerminal(isTerminal_) {
		
	}
	~Node() {};

public:
	std::vector<Node*> children;

	string code;
	string place; // actual value: 123 
	string type; // corresponding symbol in grammar : NUM

	string var_type;
	bool isTerminal;
	//GrammarEntry* entry;
};

struct Quad {
	string op;
	string arg1;
	string arg2;
	string dst;
	Quad(string op, string arg1, string arg2, string dst)
	{
		this->op = op;
		this->arg1 = arg1;
		this->arg2 = arg2;
		this->dst = dst;
	}
};
