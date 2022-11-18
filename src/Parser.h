#ifndef PARSER_H
#define PARSER_H
#include"component.h"

#include<vector>
#include<string>
#include<assert.h>

class Parser{
public:
    std::vector<GrammarEntry> grammar;

public:
    void readGrammar(const std::string& filename);
    void printGrammar(const std::string& filename);
private:
    void calFirst();
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
