#ifndef COMPONENT_H
#define COMPONENT_H

#include<vector>
#include<string>
#include<fstream>
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
    GrammarEntry() {};
    ~GrammarEntry(){};
public:

    Symbol state;
    vector<Symbol> symbols;
};

class Item{
public:
    Item(){entry=nullptr;dotPos = 0;};
    Item(GrammarEntry* entry_, int dotPos_, Symbol peek_) {
        entry = entry_;
        dotPos = dotPos_;
        peek = peek_;
    }
    ~Item(){};
public:
    const GrammarEntry* entry;
    int dotPos;
    Symbol peek;
    bool operator==(const Item &other) const {
        return (entry == other.entry)
            && (dotPos == other.dotPos)
            && (peek.type == other.peek.type);
    }

    void print(std::ofstream& f) {
        f << "Itemset " <<" \n{\n";
        f<< entry->state.type << " := "; 
        for (int i = 0; i < std::min((size_t)dotPos,entry->symbols.size()); i++) {
			auto& e = entry->symbols[i];
            f << e.type << ' ';
        }
        f << "¡£" << ' ';
        for (int i = dotPos; i < entry->symbols.size(); i++) {
            auto& e = entry->symbols[i];
            f << e.type << ' ';
        }
        f << "\n}\n";
    }
};

#endif // COMPONENT_H
