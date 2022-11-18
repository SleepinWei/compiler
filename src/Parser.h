#ifndef PARSER_H
#define PARSER_H
#include"component.h"

#include<vector>
#include<string>
#include<unordered_map>
#include<assert.h>
#include<unordered_map>
#include<set>

class Parser{
public:
    std::vector<GrammarEntry> grammar;
    std::unordered_map<Symbol, std::set<Symbol>, Symbol_hash> firstMap;
    std::unordered_map<std::string, std::vector<std::string>> firstVN;
public:
    void readGrammar(const std::string& filename);
    void printGrammar(const std::string& filename);
    void printFirst(const std::string& filename);
    void calFirstVN();
    void calFirst();
    std::vector<std::string> calFirst(const std::vector<Symbol> &rhs, size_t ofst, const Symbol &peek);
    void closure(std::vector<Item> &itemSet);
    void calFollow() {};
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
};

#endif // PARSER_H
