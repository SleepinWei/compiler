#pragma once
#include<unordered_map>
#include<string>
#include<typeinfo>
#include<assert.h>
#include"component.h"
#include"Node.h"

struct Information {
public:
	
};

enum class Type {
	INT,
	FLOAT,
	DOUBLE,
	STRING,
	CHAR
};

struct ConstInfo{
	int index;
	Type type;
};

class Generator {
public:
	Generator() { nextquad = QUAD_BEGIN; }
	~Generator() = default;

public:
	// const table

	// ·ûºÅ±í
	//std::unordered_map<std::string, Information*> symbolTable;
	int constCnt;
	std::unordered_map<std::string, ConstInfo> constTable;

	// quad
	int nextquad;
	const int QUAD_BEGIN = 100; 
	vector<Quad> quads;
	const string QUAD_EMPTY = "-";

	void constTranslation(GrammarEntry* rule,Node* root) {
		//auto id = root->left;
		//auto id1 = root->right;

		// rely on specific grammar 
		// CD->id=id1 / num
		if (rule->symbols[2].type == "num") {

		}
		//if(constTable.find())
	}
	void BoolTranslation(GrammarEntry* rule, Node* root)
	{
		;
	}
};
