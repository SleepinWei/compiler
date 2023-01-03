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
	Generator() { nextquad = QUAD_BEGIN; }
	~Generator() = default;

public:
	// quad
	int nextquad;
	const int QUAD_BEGIN = 100; 
	vector<Quad> quads;
	const string QUAD_EMPTY = "-";

	//static Generator& Instance();

	void analyze(const GrammarEntry* rule, Node* root);

	void Statement(const GrammarEntry* rule, Node* root);
};
