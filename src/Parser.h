#ifndef PARSER_H
#define PARSER_H
#include"component.h"

#include<vector>
#include<string>
#include<unordered_map>
#include<assert.h>
#include<unordered_map>
#include<set>

class Parser {
public:
    std::vector<GrammarEntry> grammar;
    std::set<Symbol> grammarSymbols;
    GrammarEntry* startEntry;
    std::unordered_map<Symbol, std::set<Symbol>, Symbol_hash> firstMap;
    std::unordered_map<std::string, std::vector<std::string>> firstVN;
    std::vector<std::vector<Item>> cluster;

    std::vector<Action> actions;
    std::vector<Goto> gotos;
    
public:
    void readGrammarYACC(const std::string& filename);
    void readGrammar(const std::string& filename);
    void printGrammar(const std::string& filename);
    void printFirst(const std::string& filename);
    void printVNFirst(const std::string& filename);
    void printCluster(const std::string& filename);
    void printTable(const std::string& filename);
    // zyw 
    void calFirst();
    //std::set<Item> calClosure(const std::set<Item>& itemSet);
    std::vector<Item> GO(const std::vector<Item>& itemSet, const std::string& x);
    void constructCluster();
    void constructTable();

	//wtc
    std::vector<std::string> calFirst(const std::vector<Symbol> &rhs, size_t ofst, const Symbol &peek);
    void closure(std::vector<Item> &itemSet);
    void calFirstVN();
};

class Loader{
public:
    static Symbol loadSymbol(const string& s){
        assert(s[0]=='<' || s[0] == '$' || s[0] == '@');
        if(s[0] == '<'){
            assert(s[s.length()-1]=='>');
            return Symbol(s,false);
        }
        else if (s[0] == '$'){
            return Symbol(s,true);
        }
        else if (s[0] == '@') {
            return Symbol(s, true, true);
        }
        assert(0);
    }
    static Symbol loadSymbolYacc(const string& s) {
        if (s[0] >= 'A' && s[0] <= 'Z' || s[0] == '\'') {
            // is terminal
            return Symbol(s, true);
        }
        else if (s[0] >= 'a' && s[0] <= 'z') {
            // not terminal
            return Symbol(s, false);
        }
    }
};

#endif // PARSER_H
