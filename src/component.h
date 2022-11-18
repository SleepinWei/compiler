#ifndef COMPONENT_H
#define COMPONENT_H

#include<vector>
#include<string>
using std::vector;
using std::string;

class Symbol{
public:
    Symbol(){};
    Symbol(const string& type,bool isTerminal,bool isEmpty = false){
        this->type = type;
        this->isTerminal = isTerminal;
        this->isEmpty = isEmpty;
    }
    ~Symbol(){};
public:
    string type;
    string value;
    bool isTerminal;
    bool isEmpty;
};

class GrammarEntry{
public:
    GrammarEntry(const Symbol& state,const vector<Symbol>& symbols){
        this->state = state;
        this->symbols = symbols;
    }
    ~GrammarEntry(){};
public:

    Symbol state;
    vector<Symbol> symbols;
};

class Item{
public:
    Item(){entry=nullptr;dotPos = 0;};
    ~Item(){};
public:
    GrammarEntry* entry;
    int dotPos;
};

#endif // COMPONENT_H
