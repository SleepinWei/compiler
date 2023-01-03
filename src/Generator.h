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
	Generator() { nextquad = 100; }
	~Generator() = default;

public:
	// const table

	// ·ûºÅ±í
	//std::unordered_map<std::string, Information*> symbolTable;
	int constCnt;
	std::unordered_map<std::string, ConstInfo> constTable;

	// quad
	int nextquad;

	void constTranslation(GrammarEntry* rule,Node* root) {
		//auto id = root->left;
		//auto id1 = root->right;

		// rely on specific grammar 
		// CD->id=id1 / num
		if (rule->symbols[2].type == "num") {

		}
		//if(constTable.find())
	}
};
