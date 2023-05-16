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
	string label;
	Quad(string op, string arg1, string arg2, string dst,string label="")
	{
		this->op = op;
		this->arg1 = arg1;
		this->arg2 = arg2;
		this->dst = dst;
		this->label = label;
	}
};

// 符号表表项
struct SymbolEntryVar{
	string name; 
	string type;
	int offset; // 对形式参数，为负值
	int size; 
	bool is_array; 
	vector<int> dims;  // array dims 
};

struct SymbolEntryConst {
	string type;
	string val; 
};

struct SymbolTable{
	SymbolTable* parent = nullptr;
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

SymbolTable* mktable(SymbolTable* parent);
void enter(SymbolTable* table, string name, string type, int offset,int size, bool is_array = false, vector<int> dims = vector<int>());
void addWidth(SymbolTable* table, int width);
SymbolTable* enterproc(SymbolTable* table);
