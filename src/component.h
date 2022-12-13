#ifndef COMPONENT_H
#define COMPONENT_H

#include<vector>
#include<string>
#include<fstream>
#include<iostream>
#include<ostream>
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

    void print(std::ofstream& cout) {
        cout << state.type << " := ";
        for (auto& sym : symbols) {
            cout << sym.type << ' ';
        }
        cout << '\n';
    }
    void print(std::ostream& cout) const {
        cout << state.type << " := ";
        for (auto& sym : symbols) {
            cout << sym.type << ' ';
        }
        cout << '\n';
    }
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
    GrammarEntry* entry;
    int dotPos;
    Symbol peek;

    bool operator==(const Item &other) const {
        return (entry == other.entry)
            && (dotPos == other.dotPos)
            && (peek.type == other.peek.type);
    }
    bool operator<(const Item& other)const {
        if ((int)entry < (int)other.entry) {
            return true;
        }
        else if ((int)entry > (int)other.entry) {
            return false;
        }
        if (dotPos < other.dotPos)
        {
            return true;
        }
        else if (dotPos > other.dotPos) {
            return false;
        }
        if (peek.type < other.peek.type) {
            return true;
        }
        return false;
    }

    void print(std::ofstream& f) const {
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

        f << "peek: " << peek.type << '\n';
    }
};

class TableEntry{
public:
    TableEntry() {
        useStack = false;
        destState = -1;
        gen = nullptr;
        isAcc = false;
    }
    TableEntry(bool useStack_, int destState_,GrammarEntry* gen_,bool isAcc_) {
        useStack = useStack_;
        destState = destState_;
        this->gen = gen_;
        isAcc = isAcc_;
    }
    ~TableEntry() {};

    bool operator==(const TableEntry& a) {
        return (useStack == a.useStack) && (destState == a.destState) && (gen == a.gen) && (isAcc == a.isAcc);
    }

public:
    //int state; // current state
    //std::string inString; // incoming string   
    bool useStack; // sj or rj
    int destState; // sj or rj 
    GrammarEntry* gen; 
    bool isAcc;
};

//class Goto {
//public:
//    Goto() {};
//    Goto(int state_, string inState_, int gotoState_) {
//        state = state_;
//        inState = inState_;
//        gotoState = gotoState_;
//    }
//    ~Goto() {};
//
//    bool operator==(const Goto& g) {
//        return state == g.state && inState == g.inState && gotoState == g.gotoState;
//    }
//public:
//    int state;
//    std::string inState;
//    int gotoState;
//};

#endif // COMPONENT_H
