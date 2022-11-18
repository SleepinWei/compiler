#ifndef PARSER_H
#define PARSER_H
#include"component.h"

#include<vector>
#include<string>

class Parser{
public:
    std::vector<GrammarEntry> grammar;
public:
    void readGrammar(const std::string& filename);
private:
    void calFirst();
    void calFollow();
};

#endif // PARSER_H
