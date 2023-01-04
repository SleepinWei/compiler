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
	Generator() { tempCnt = 0; }
	~Generator() = default;

public:
	// quad
	int nextquad() { return quads.size() + QUAD_BEGIN; }
	const int QUAD_BEGIN = 100; 
	vector<Quad> quads;
	const string QUAD_EMPTY = "-";

	// temp
	int tempCnt;
	string newtemp();
	void backpatch(std::vector <int>& list_, int q)
	{
		for (auto& i : list_)
		{
			quads[i - QUAD_BEGIN].dst = std::to_string(q);
		}
	}
	//static Generator& Instance();

	void analyze(const GrammarEntry* rule, Node* root);

	void Statement(const GrammarEntry* rule, Node* root);

	void Assignment(const GrammarEntry* rule, Node* root);

	void BoolExpression(const GrammarEntry* rule, Node* root);

	void output(const string& filename);
	void Iteration(const GrammarEntry* rule, Node* root);

	void Mquad(const GrammarEntry* rule, Node* root);

	void Selection(const GrammarEntry* rule, Node* root);
};
