#pragma once
#include"component.h"
class GrammarEntry;
#include<map>

class Node {
public:
	Node() { isTerminal = false; width = 0; };
	Node(string type_, string place_,bool isTerminal_,int width_ = 0):type(type_),place(place_),isTerminal(isTerminal_) {
		width = width_;
	}
	~Node() {};

public:
	std::vector<Node*> children;

	string code;
	string place; // actual value: 123 
	string type; // corresponding symbol in grammar : NUM
	int width;

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

// ·ûºÅ±í±íÏî
struct SymbolEntryVar{
	string type;
	int offset;
};

struct SymbolEntryConst {
	string type;
	string val; 
};

struct SymbolEntryFunction {

};

struct SymbolTable{
	SymbolTable* previous = nullptr;
	int offset = 0;
	int width = 0;
	std::map<string,SymbolEntryVar> symbols; // (name, type, offset)
	std::map<string, SymbolEntryConst> consts; 
};

struct FunctionEntry{
	int addr; 
	string ret_type; 
	vector<string> args; 
};

using FunctionTable = std::map<string, FunctionEntry>;

SymbolTable* mktable(SymbolTable* previous);
void enter(SymbolTable* table, string name, string type, int offset);
void addWidth(SymbolTable* table, int width);
SymbolTable* enterproc(SymbolTable* table);
