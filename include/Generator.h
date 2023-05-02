#pragma once
#include<unordered_map>
#include<string>
#include<typeinfo>
#include<assert.h>
#include<stack>
#include"component.h"

class Node;
#include"Node.h"

class IR {
public:
	IR() {
		tempCnt = 0; 
		globalTable = mktable(nullptr);
		functionTable = new FunctionTable();
		curTable = globalTable;
	}

	~IR() {
		for (auto& iter : symbolTables) {
			delete iter.second; 
		}
		if (globalTable)
			delete globalTable;
		if (functionTable) {
			delete functionTable;
		}
	}
public:
	int tempCnt;
	vector<Quad> quads; 
	//std::stack<SymbolTable*> symbolTableStack;

	SymbolTable* curTable; 
	std::map<string, SymbolTable*> symbolTables; 
	SymbolTable* globalTable; 
	FunctionTable* functionTable; 

	const int QUAD_BEGIN = 100; 
	const string QUAD_EMPTY = "-";

public:
	static IR* Create() {
		return new IR(); 
	}
	static void Destroy(IR* ir) {
		delete ir; 
	}

	int nextquad() { return quads.size() + QUAD_BEGIN; }
	string newtemp();
	void backpatch(std::vector <int>& list_, int q)
	{
		for (auto& i : list_)
		{
			quads[i - QUAD_BEGIN].dst = std::to_string(q);
		}
	}

};

class Generator {
public:
	static std::map<string, int> TYPE_WIDTH; 
private:
	Generator() = default;
	~Generator() = default;

public:
	static Generator& GetInstance(); 
	void analyze(IR* ir, const GrammarEntry* rule, Node* root);
	void output(IR* ir, const string& filename);

private:
	void Statement(IR* ir, const GrammarEntry* rule, Node* root);

	void Assignment(IR* ir, const GrammarEntry* rule, Node* root);

	void BoolExpression(IR* ir, const GrammarEntry* rule, Node* root);

	void Function(IR* ir, const GrammarEntry* rule, Node* root);

	void Iteration(IR* ir, const GrammarEntry* rule, Node* root);

	void Mquad(IR* ir, const GrammarEntry* rule, Node* root);

	void Selection(IR* ir, const GrammarEntry* rule, Node* root);
};

