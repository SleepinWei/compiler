#ifndef PARSER_H
#define PARSER_H
#include"component.h"

#include<vector>
#include<string>
#include<unordered_map>
#include<assert.h>

class Parser{
public:
    std::vector<GrammarEntry> grammar;
    std::unordered_map<std::string, std::vector<std::string>> firstVN;
public:
    void readGrammar(const std::string& filename);
    void printGrammar(const std::string& filename);
private:
    void calFirstVN();
    void calFirst();
    void closure(std::vector<Item> &itemSet);
    void calFollow() {};
};

class Loader{
public:
    static Symbol loadSymbol(const string& s){
        assert(s[0]=='<' || s[0] == '$' || s[0] == '@');
        if(s[0] == '<'){
            assert(s[s.length()-1]=='>');
            return Symbol(s.substr(1,s.length()-2),false);
        }
        else if (s[0] == '$'){
            return Symbol(s.substr(1,s.length()-1),true);
        }
        else if (s[0] == '@') {
            return Symbol(s.substr(1, s.length() - 1), true, true);
        }
        assert(0);
    }
};

#endif // PARSER_H
