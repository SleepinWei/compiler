#ifndef COMPONENT_H
#define COMPONENT_H

#include<vector>
#include<string>
using std::vector;
using std::string;

class Symbol{
public:
    Symbol();
    ~Symbol();
public:
};

class Token:public Symbol{
public:
    Token();
    ~Token();
public:
    string type; // token types: int, ....
    string name; // actual charaters
};

class State:public Symbol{
public:
    State();
    ~State();
    bool operator==(const State& state)const {
        return type == state.type;
    }
public:
    string type;
};

class GrammarEntry{
public:
    GrammarEntry();
    ~GrammarEntry();
public:
    string state;
    vector<Symbol> symbols;
};

class Item{
public:
    Item();
    ~Item();
public:
    GrammarEntry* entry;
    int dotPos;
    string peek;
    bool operator==(const Item &other) const {
        return (entry == other.entry)
            && (dotPos == other.dotPos)
            && (peek == other.peek);
    }
};

#endif // COMPONENT_H
