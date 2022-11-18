#ifndef COMPONENT_H
#define COMPONENT_H

#include<vector>
#include<string>
#include<utility>
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

    bool operator==(const Symbol& s) const {
        return type == s.type;
    }
    bool operator<(const Symbol& s)const {
        return type < s.type;
    }
public:
    string type;
    string value;
    bool isTerminal;
    bool isEmpty;
};

struct Symbol_hash {
    size_t operator()(const Symbol& s) const {
        return std::hash<string>()(s.type);
    }
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
