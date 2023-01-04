#pragma once
#include<unordered_map>
#include<string>
#include<typeinfo>
#include<assert.h>
#include"component.h"

class Node;

class Generator {
public:
	static Generator generator;
	Generator() { nextquad = QUAD_BEGIN; tempCnt = 0; }
	~Generator() = default;

public:
	// quad
	int nextquad;
	const int QUAD_BEGIN = 100; 
	vector<Quad> quads;
	const string QUAD_EMPTY = "-";

	// temp
	int tempCnt;
	string newtemp();

	//static Generator& Instance();

	void analyze(const GrammarEntry* rule, Node* root);

	void Statement(const GrammarEntry* rule, Node* root);

	void Assignment(const GrammarEntry* rule, Node* root);

	void output(const string& filename);
};
